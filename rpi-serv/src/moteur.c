#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "moteur.h"

#define PWM_CHIP   "/sys/class/pwm/pwmchip0"
#define PWM_CH     "pwm0"
#define PWM_DUTY   "/sys/class/pwm/pwmchip0/pwm0/duty_cycle"
#define PWM_PERIOD "/sys/class/pwm/pwmchip0/pwm0/period"
#define PWM_ENABLE "/sys/class/pwm/pwmchip0/pwm0/enable"



void moteur_control(float commande) {
    if (commande < 0.0f)
        commande = 0.0f;
    if (commande > 1.0f)
        commande = 1.0f;
    int pulse_ns = 1400000 + (int)(commande * 200000.0f);
    char cmd[200];
    snprintf(cmd, sizeof(cmd), "echo %d > %s", pulse_ns, PWM_DUTY);
    system(cmd);
    printf("Commande = %.2f -> impulsion = %.3f ms\n", commande, pulse_ns / 1000000.0f);
}


int moteur_init(void) {
    printf("\nInitialisation du PWM moteur...\n");
    system("echo 0 > /sys/class/pwm/pwmchip0/export 2>/dev/null");
    usleep(100000);
    system("echo 0 > /sys/class/pwm/pwmchip0/pwm0/enable 2>/dev/null");
    system("echo 20000000 > /sys/class/pwm/pwmchip0/pwm0/period");
    system("echo 1500000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle");
    printf("Configuration GPIO12 -> PWM0_CHAN0...\n");
    system("pinctrl set 12 a0");
    printf("\nEtat du GPIO12 :\n");
    system("pinctrl get 12");
    system("echo 1 > /sys/class/pwm/pwmchip0/pwm0/enable");
    printf("\nPWM actif : 50 Hz\n");
    printf("Periode : 20 ms\n");
    printf("GPIO : 12\n");
    printf("Fonction : ALT0 (a0)\n");
    printf("PWM : PWM0_CHAN0\n");
    printf("Canal Linux : pwm0\n");
    printf("Moteur au neutre : 1.500 ms\n");
    printf("\nInitialisation de l'ESC au neutre...\n");
    sleep(3);
    return 0;
}
