#include <stdio.h>
#include <unistd.h>
#include "servo.h"

#define PWM_PATH "/sys/class/pwm/pwmchip0"
#define PERIOD_NS 20000000L // 20 ms => 50 Hz

#define ANGLE_MIN_DEG 55
#define ANGLE_MAX_DEG 145
#define ANGLE_CENTER_DEG ((ANGLE_MIN_DEG + ANGLE_MAX_DEG) / 2)     
#define ANGLE_HALF_RANGE_DEG ((ANGLE_MAX_DEG - ANGLE_MIN_DEG) / 2.0f)

static int initialized = 0; 

static void write_sysfs(const char *path, long value) {
    FILE *f = fopen(path, "w");
    if (!f) { perror(path); return; }
    fprintf(f, "%ld", value);
    fclose(f);
}

static long angle_to_duty_ns(int angle) {
    if (angle < 0)   angle = 0;
    if (angle > 180) angle = 180;
    return 1000000L + (angle * 1000000L) / 180;
}

static void init_once(void) {
    if (initialized) return;

    FILE *f = fopen(PWM_PATH "/export", "w");
    if (f) { fprintf(f, "1"); fclose(f); usleep(100000); } // GPIO13

    write_sysfs(PWM_PATH "/pwm1/period", PERIOD_NS);
    write_sysfs(PWM_PATH "/pwm1/duty_cycle", 1500000);
    write_sysfs(PWM_PATH "/pwm1/enable", 1);

    initialized = 1;
}

void servo_control(float value) {
    init_once();

    if (value < -1.0f) value = -1.0f;
    if (value >  1.0f) value =  1.0f;

    int angle = (int)(ANGLE_CENTER_DEG + value * ANGLE_HALF_RANGE_DEG);
    if (angle < ANGLE_MIN_DEG) angle = ANGLE_MIN_DEG;
    if (angle > ANGLE_MAX_DEG) angle = ANGLE_MAX_DEG;

    write_sysfs(PWM_PATH "/pwm1/duty_cycle", angle_to_duty_ns(angle));
}
