/*
 * server.c
 * Tourne sur le Raspberry Pi 5 : cree le point d'acces Wi-Fi (hostapd + dnsmasq)
 * sur l'antenne USB, puis ecoute en UDP et affiche chaque message recu,
 * en separant la partie texte de la partie nombre (format "texte:nombre").
 *
 * Configuration par variables d'environnement (voir docker-compose.yml) :
 *   AP_IFACE, AP_SSID, AP_PASS, AP_IP, AP_DHCP_START, AP_DHCP_END, AP_CHANNEL
 *
 * Utilisation : ./serveur_pi <port>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>

#define HOSTAPD_CONF "/tmp/hostapd.conf"

// Valeur d'une variable d'environnement, ou valeur par defaut si absente
static const char *env_or(const char *name, const char *def) {
    const char *v = getenv(name);
    return (v != NULL && v[0] != '\0') ? v : def;
}

// Verifie qu'une chaine est bien une adresse IPv4 (evite les fautes de frappe)
static void check_ip(const char *label, const char *ip) {
    struct in_addr tmp;
    if (inet_pton(AF_INET, ip, &tmp) != 1) {
        fprintf(stderr, "Erreur : %s=\"%s\" n'est pas une adresse IPv4 valide\n", label, ip);
        exit(1);
    }
}

// Lance un programme en arriere-plan et retourne son pid
static pid_t spawn(char *const argv[]) {
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); exit(1); }
    if (pid == 0) {
        execvp(argv[0], argv);
        perror(argv[0]);
        _exit(127);
    }
    return pid;
}

// Si hostapd ou dnsmasq meurt, on quitte : Docker relancera tout proprement
static void un_demon_est_mort(int sig) {
    (void)sig;
    _exit(1);
}

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IOLBF, 0); // sinon les printf restent invisibles dans docker logs

    if (argc != 2) {
        fprintf(stderr, "Usage : %s <port>\n", argv[0]);
        return 1;
    }
    int port = atoi(argv[1]);

    // ----- 1. Configuration (surchargable via docker-compose.yml) -----
    const char *iface      = env_or("AP_IFACE", "wlan0");
    const char *ssid       = env_or("AP_SSID", "PiVoiture");
    const char *pass       = env_or("AP_PASS", "drace_wifi");
    const char *ip         = env_or("AP_IP", "192.168.4.1");
    const char *dhcp_start = env_or("AP_DHCP_START", "192.168.4.10");
    const char *dhcp_end   = env_or("AP_DHCP_END", "192.168.4.100");
    const char *channel    = env_or("AP_CHANNEL", "1");

    check_ip("AP_IP", ip);
    check_ip("AP_DHCP_START", dhcp_start);
    check_ip("AP_DHCP_END", dhcp_end);
    if (strlen(pass) < 8) {
        fprintf(stderr, "Erreur : AP_PASS doit faire au moins 8 caracteres (exigence WPA2)\n");
        return 1;
    }

    // Attend que l'interface existe (antenne USB debranchee, ou pas encore enumeree au boot)
    char syspath[128];
    snprintf(syspath, sizeof(syspath), "/sys/class/net/%s", iface);
    while (access(syspath, F_OK) != 0) {
        printf("En attente de l'interface %s (antenne USB branchee ?)...\n", iface);
        sleep(3);
    }

    // ----- 2. Adresse IP statique sur l'interface de l'AP -----
    char cmd[256];
    snprintf(cmd, sizeof(cmd),
             "ip link set %s up && ip addr flush dev %s && ip addr add %s/24 dev %s",
             iface, iface, ip, iface);
    if (system(cmd) != 0) {
        fprintf(stderr, "Erreur : impossible de configurer %s (interface absente ? cap NET_ADMIN ?)\n", iface);
        return 1;
    }
    printf("Interface %s configuree en %s/24\n", iface, ip);

    // ----- 3. hostapd : cree le reseau Wi-Fi -----
    FILE *f = fopen(HOSTAPD_CONF, "w");
    if (f == NULL) { perror(HOSTAPD_CONF); return 1; }
    fprintf(f,
            "interface=%s\n"
            "driver=nl80211\n"
            "ssid=%s\n"
            "country_code=FR\n"
            "hw_mode=g\n"        // 2.4 GHz : meilleure portee que le 5 GHz
            "channel=%s\n"
            "ieee80211n=1\n"
            "wmm_enabled=1\n"
            "auth_algs=1\n"
            "wpa=2\n"
            "wpa_key_mgmt=WPA-PSK\n"
            "rsn_pairwise=CCMP\n"
            "wpa_passphrase=%s\n",
            iface, ssid, channel, pass);
    fclose(f);

    signal(SIGCHLD, un_demon_est_mort); // apres les system(), avant les spawn()

    char *hostapd_argv[] = {"hostapd", HOSTAPD_CONF, NULL};
    spawn(hostapd_argv);
    printf("hostapd lance : SSID \"%s\", canal %s\n", ssid, channel);
    sleep(2); // laisse hostapd monter l'interface avant dnsmasq

    // ----- 4. dnsmasq : distribue les adresses IP aux clients (DHCP) -----
    char arg_iface[64], arg_range[128];
    snprintf(arg_iface, sizeof(arg_iface), "--interface=%s", iface);
    snprintf(arg_range, sizeof(arg_range), "--dhcp-range=%s,%s,255.255.255.0,12h", dhcp_start, dhcp_end);
    char *dnsmasq_argv[] = {"dnsmasq", "--keep-in-foreground", "--bind-interfaces",
                            "--port=0",           // pas de serveur DNS, seulement DHCP
                            "--log-facility=-",   // logs vers stdout et non syslog
                            "--log-dhcp",
                            arg_iface, arg_range, NULL};
    spawn(dnsmasq_argv);
    printf("dnsmasq lance : DHCP de %s a %s\n", dhcp_start, dhcp_end);

    // ----- 5. Reception UDP -----
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; // ecoute sur toutes les interfaces (dont l'AP)
    addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sock);
        return 1;
    }

    // Timeout de reception : permet de verifier regulierement que l'antenne est toujours la
    struct timeval tv = { .tv_sec = 5, .tv_usec = 0 };
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    printf("En ecoute UDP sur le port %d...\n", port);

    char buffer[256];
    struct sockaddr_in expediteur;

    while (1) {
        socklen_t len = sizeof(expediteur);
        ssize_t recus = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                                 (struct sockaddr *)&expediteur, &len);
        if (recus < 0) {
            if (access(syspath, F_OK) != 0) {
                fprintf(stderr, "Interface %s disparue (antenne debranchee ?) : redemarrage\n", iface);
                return 1; // Docker relance le conteneur, qui re-attendra l'interface
            }
            if (errno != EAGAIN && errno != EWOULDBLOCK) perror("recvfrom");
            continue;
        }
        buffer[recus] = '\0';
        buffer[strcspn(buffer, "\r\n")] = '\0'; // retire le retour a la ligne final

        char ip_expediteur[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &expediteur.sin_addr, ip_expediteur, sizeof(ip_expediteur));

        // On separe "texte:nombre" (le nombre est un flottant, ex. "JGX:-0.53")
        char *sep = strchr(buffer, ':');
        if (sep != NULL) {
            *sep = '\0';
            float valeur = strtof(sep + 1, NULL);
            printf("De %s -> texte=\"%s\" valeur=%.2f\n", ip_expediteur, buffer, valeur);
        } else {
            printf("De %s -> message brut : %s\n", ip_expediteur, buffer);
        }
    }

    close(sock);
    return 0;
}