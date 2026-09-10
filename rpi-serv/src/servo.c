#include "servo.h"
#include <stdio.h>
#include <unistd.h>

/*
 * PWM materiel du Pi 5 (kernel 6.6) : pwmchip2, GPIO13 = canal 1 (rapport §3.2.1).
 * Prerequis sur l'hote : dtoverlay=pwm-2chan,pin=12,func=4,pin2=13,func2=4
 */
#define PWM_CHIP   "/sys/class/pwm/pwmchip2"
#define PWM_CHAN   1
#define PWM_DIR    PWM_CHIP "/pwm1"
#define PERIOD_NS  20000000L    /* 50 Hz */
#define NEUTRAL_NS 1500000L     /* 1.5 ms : roues droites */
#define RANGE_NS   500000L      /* 1.0 a 2.0 ms : butees gauche / droite */

static int servo_pret = 0;

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

void servo_init(void)
{
    if (access(PWM_DIR, F_OK) != 0) {           /* canal pas encore exporte */
        if (sysfs_write(PWM_CHIP "/export", PWM_CHAN) < 0) {
            fprintf(stderr, "PWM inaccessible : direction desactivee\n");
            return;
        }
        usleep(100000);                          /* laisse le noyau creer pwm1/ */
    }

    if (pwm_write("period", PERIOD_NS) < 0 ||
        pwm_write("duty_cycle", NEUTRAL_NS) < 0 ||
        pwm_write("enable", 1) < 0)
        return;

    servo_pret = 1;
    printf("Servo : PWM 50 Hz sur GPIO13 (canal %d), centre 1.5 ms\n", PWM_CHAN);
}

void servo_control(float value)
{
    if (!servo_pret)
        return;

    /* Saturation */
    if (value < -1.0f)
        value = -1.0f;
    if (value > 1.0f)
        value = 1.0f;

    pwm_write("duty_cycle", NEUTRAL_NS + (long)(value * RANGE_NS));
}

