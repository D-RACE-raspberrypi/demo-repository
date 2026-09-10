#include "moteur.h"
#include <stdio.h>
#include <unistd.h>

/*
 * PWM materiel du Pi 5 (kernel 6.6) : pwmchip2, GPIO12 = canal 0 (rapport §3.2.1).
 * Prerequis sur l'hote : dtoverlay=pwm-2chan,pin=12,func=4,pin2=13,func2=4
 */
#define PWM_CHIP   "/sys/class/pwm/pwmchip2"
#define PWM_CHAN   0
#define PWM_DIR    PWM_CHIP "/pwm0"
#define PERIOD_NS  20000000L    /* 50 Hz */
#define NEUTRAL_NS 1500000L     /* 1.5 ms : neutre ESC */
#define RANGE_NS   100000L      /* +/- 0.1 ms : puissance volontairement bridee */

static int moteur_pret = 0;

static int sysfs_write(const char *path, long value)
{
    FILE *f = fopen(path, "w");
    if (f == NULL) {
        perror(path);
        return -1;
    }
    fprintf(f, "%ld", value);
    if (fclose(f) != 0) {       /* le noyau remonte l'erreur a l'ecriture reelle */
        perror(path);
        return -1;
    }
    return 0;
}

static int pwm_write(const char *file, long value)
{
    char path[128];
    snprintf(path, sizeof(path), PWM_DIR "/%s", file);
    return sysfs_write(path, value);
}

void moteur_init(void)
{
    if (access(PWM_DIR, F_OK) != 0) {           /* canal pas encore exporte */
        if (sysfs_write(PWM_CHIP "/export", PWM_CHAN) < 0) {
            fprintf(stderr, "PWM inaccessible : moteur desactive\n");
            return;
        }
        usleep(100000);                          /* laisse le noyau creer pwm0/ */
    }

    if (pwm_write("period", PERIOD_NS) < 0 ||
        pwm_write("duty_cycle", NEUTRAL_NS) < 0 ||
        pwm_write("enable", 1) < 0)
        return;

    moteur_pret = 1;
    printf("Moteur : PWM 50 Hz sur GPIO12 (canal %d), neutre 1.5 ms\n", PWM_CHAN);
}

void moteur_control(float value)
{
    if (!moteur_pret)
        return;

    /* Saturation */
    if (value < -1.0f)
        value = -1.0f;
    if (value > 1.0f)
        value = 1.0f;

    pwm_write("duty_cycle", NEUTRAL_NS + (long)(value * RANGE_NS));
}
