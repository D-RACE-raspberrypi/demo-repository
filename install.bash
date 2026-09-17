#!/bin/bash
# Arrete immediatement le script si une commande echoue
set -e

# Definition des couleurs pour la console
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0;37m' # Pas de couleur (Reset)

echo -e "${BLUE}========================================================${NC}"
echo -e "${BLUE}  Starting D-RACE Raspberry Pi Installation Script      ${NC}"
echo -e "${BLUE}========================================================${NC}"

# Recuperation du vrai utilisateur (celui qui a lance le sudo)
REAL_USER=${SUDO_USER:-$USER}
HOME_DIR="/home/${REAL_USER}"

# 1. Verification des privileges Root
if [ "$EUID" -ne 0 ]; then
  echo -e "${RED}[ERROR] Veuillez lancer ce script avec sudo (ex: sudo ./install.sh)${NC}"
  exit 1
fi

# 2. Nettoyage FORCE des anciens fichiers APT parasites
echo -e "${YELLOW}-> Purging old and invalid Docker repositories...${NC}"
# Supprime de maniere agressive tous les fichiers lie a docker dans les listes de sources
sudo rm -f /etc/apt/sources.list.d/*docker*.list
sudo rm -f /etc/apt/sources.list.d/*docker*

# Nettoyage absolu des lignes contenant docker.com dans le fichier principal si existant
if [ -f /etc/apt/sources.list ]; then
    sudo grep -v "docker.com" /etc/apt/sources.list > /tmp/sources.list.tmp || true
    sudo mv /tmp/sources.list.tmp /etc/apt/sources.list
fi

# 3. Installation des dependances initiales
echo -e "${YELLOW}-> Updating package lists and installing prerequisites...${NC}"
apt-get update
apt-get install -y ca-certificates curl gnupg rfkill

echo -e "${YELLOW}-> Setting up Docker keyrings and repository...${NC}"
install -m 0755 -d /etc/apt/keyrings

# 1. Votre ligne curl correcte (officielle) :
curl -fsSL https://download.docker.com/linux/debian/gpg | tee /etc/apt/keyrings/docker.asc > /dev/null
chmod a+r /etc/apt/keyrings/docker.asc

# Detection de la version
OS_CODENAME=$(. /etc/os-release && echo "$VERSION_CODENAME")
if [ "$OS_CODENAME" = "trixie" ]; then
    echo "[INFO] Distribution Trixie detectee. Utilisation du depot stable Bookworm pour Docker."
    OS_CODENAME="bookworm"
fi

# 2. La ligne de depot deb TOUTE AUSSI IMPORTANTE à vérifier (il faut bien "download.docker.com") :
echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.asc] https://download.docker.com/linux/debian ${OS_CODENAME} stable" | tee /etc/apt/sources.list.d/docker.list > /dev/null

echo -e "${YELLOW}-> Removing old Docker buildx plugin if it exists...${NC}"
apt-get remove -y docker-buildx || true

echo -e "${YELLOW}-> Updating package index with the new repository...${NC}"
apt-get update

echo -e "${YELLOW}-> Installing Docker packages...${NC}"
apt-get install -y docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin

# 4. Configuration des droits Docker
echo -e "${YELLOW}-> Adding user ${REAL_USER} to the Docker group...${NC}"
usermod -aG docker ${REAL_USER}

echo -e "${YELLOW}-> Fixing Docker socket permissions...${NC}"
chown root:docker /var/run/docker.sock
chmod 660 /var/run/docker.sock

# Regle persistante pour garder les droits sur le socket apres reboot
echo "f /var/run/docker.sock 0660 root docker -" | tee /etc/tmpfiles.d/docker-socket.conf > /dev/null
echo -e "${GREEN}[SUCCESS] Docker installation complete.${NC}"

# 5. Clone du depot Git distant
echo -e "${YELLOW}-> Cloning demo repository...${NC}"
cd "${HOME_DIR}"
if [ ! -d "${HOME_DIR}/demo-repository" ]; then
    sudo -u ${REAL_USER} git clone https://github.com/D-RACE-raspberrypi/demo-repository.git
    echo -e "${GREEN}[SUCCESS] Demo repository cloned.${NC}"
else
    echo -e "${BLUE}[INFO] Le dossier demo-repository existe deja. Etape ignoree.${NC}"
fi

# 6. Creation du service de demarrage Systemd
echo -e "${YELLOW}-> Setting up rpi-serv systemd service...${NC}"
tee /etc/systemd/system/rpi-serv.service > /dev/null <<EOL
[Unit]
Description=Demarrage automatique du projet Docker Compose
Requires=docker.service
After=docker.service network-online.target
Wants=network-online.target

[Service]
Type=oneshot
RemainAfterExit=yes
WorkingDirectory=${HOME_DIR}/demo-repository/rpi-serv
ExecStart=/usr/bin/docker compose up -d
ExecStop=/usr/bin/docker compose down

[Install]
WantedBy=multi-user.target
EOL

echo -e "${YELLOW}-> Activating rpi-serv systemd service...${NC}"
systemctl daemon-reload
systemctl enable rpi-serv.service
echo -e "${GREEN}[SUCCESS] rpi-serv systemd service setup complete.${NC}"

# 7. Configuration du PWM (Materiel)
echo -e "${YELLOW}-> Adding dtoverlay for PWM...${NC}"
CONFIG_PATH="/boot/config.txt"
if [ -d "/boot/firmware" ]; then
    CONFIG_PATH="/boot/firmware/config.txt"
fi

sed -i '/^dtoverlay=pwm2-chan/d' "$CONFIG_PATH"
echo "dtoverlay=pwm-2chan,pin=12,func=4,pin2=13,func2=4" | tee -a "$CONFIG_PATH"
echo -e "${GREEN}[SUCCESS] dtoverlay for PWM added in $CONFIG_PATH.${NC}"

# 8. Attribution persistante des cartes Wi-Fi (udev)
echo -e "${YELLOW}-> Fixing permanent Wi-Fi interface names (udev)...${NC}"
MAC_INTERNAL=$(cat /sys/class/net/wlan0/address 2>/dev/null || true)
MAC_EXTERNAL=$(cat /sys/class/net/wlan1/address 2>/dev/null || true)

if [ ! -z "$MAC_INTERNAL" ] && [ ! -z "$MAC_EXTERNAL" ]; then
    tee /etc/udev/rules.d/70-persistent-net.rules > /dev/null <<EOL
# Carte Wi-Fi Interne / Principale
SUBSYSTEM=="net", ACTION=="add", DRIVERS=="?*", ATTR{address}=="$MAC_INTERNAL", NAME="wlan0"

# Carte Wi-Fi Externe / Point d'Acces (drace_serv)
SUBSYSTEM=="net", ACTION=="add", DRIVERS=="?*", ATTR{address}=="$MAC_EXTERNAL", NAME="wlan1"
EOL
    echo -e "${GREEN}[SUCCESS] Udev rules created successfully.${NC}"
else
    echo -e "${YELLOW}[WARNING] Less than two Wi-Fi interfaces detected. Skipping permanent naming.${NC}"
fi

# 9. Configuration pays Wi-Fi et deblocage de l'antenne
echo -e "${YELLOW}-> Setting Wi-Fi Country to FR...${NC}"
mkdir -p /etc/wpa_supplicant
if [ ! -f /etc/wpa_supplicant/wpa_supplicant.conf ]; then
    echo "ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev" | tee /etc/wpa_supplicant/wpa_supplicant.conf > /dev/null
fi
sed -i '/^country=/d' /etc/wpa_supplicant/wpa_supplicant.conf
echo "country=FR" | tee -a /etc/wpa_supplicant/wpa_supplicant.conf

# Forcer l'application immediate au niveau du Kernel
iw reg set FR || true
rfkill unblock wifi || true
echo -e "${GREEN}[SUCCESS] Wi-Fi Country set to FR and hardware unblocked.${NC}"

# 10. Isolation de wlan1 (Desactivation de la gestion par l'hote)
echo -e "${YELLOW}-> Isolating wlan1 from host network management (NetworkManager)...${NC}"
mkdir -p /etc/NetworkManager/conf.d
tee /etc/NetworkManager/conf.d/99-ignore-wlan1.conf > /dev/null <<EOL
[keyfile]
unmanaged-devices=interface-name:wlan1
EOL
systemctl restart NetworkManager || true
echo -e "${YELLOW}-> Waiting for network connectivity...${NC}"
for i in {1..60}; do
    if getent hosts docker.io >/dev/null 2>&1; then
        echo -e "${GREEN}[SUCCESS] Network is back.${NC}"
        break
    fi
    sleep 1
done
if ! getent hosts docker.io >/dev/null 2>&1; then
    echo -e "${RED}[ERROR] Network connectivity could not be established.${NC}"
fi
echo -e "${GREEN}[SUCCESS] wlan1 successfully isolated.${NC}"

# 11. Build de l'image et premier lancement des conteneurs
echo -e "${YELLOW}-> Building rpi-serv Docker container...${NC}"
docker compose -f ${HOME_DIR}/demo-repository/rpi-serv/docker-compose.yml build

echo -e "${YELLOW}-> Starting rpi-serv Docker container...${NC}"
docker compose -f ${HOME_DIR}/demo-repository/rpi-serv/docker-compose.yml up -d

echo -e "${BLUE}========================================================${NC}"
echo -e "${GREEN}[SUCCESS] Installation script completed successfully!  ${NC}"
echo -e "${BLUE}========================================================${NC}"

# 12. Redemarrage controle
echo "Un redemarrage est necessaire pour appliquer les configurations du groupe Docker, du PWM et du Wi-Fi."
read -p "Souhaitez-vous redemarrer le Raspberry Pi maintenant ? (y/n) : " reponse
if [[ "$reponse" =~ ^[YyLlOo] ]]; then
    echo "Fermeture propre de la session utilisateur et redemarrage..."
    pkill -u ${REAL_USER} -f sshd || true
    sleep 1
    reboot
else
    echo "Prenez soin de taper 'sudo reboot' manuellement pour appliquer les modifications."
fi
