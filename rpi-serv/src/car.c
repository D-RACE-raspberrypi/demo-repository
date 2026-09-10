#include "car.h"
#include <stdio.h>
#include <string.h>

// GPIO ouverts une fois par car_init ; restent NULL si indisponibles (ex. test sur PC)
static struct gpiod_chip *chip = NULL;
static struct gpiod_line *led_gauche = NULL, *led_droite = NULL,
                         *led_frein = NULL, *led_accel = NULL;

static struct gpiod_line *led_request(unsigned int pin) {
    struct gpiod_line *line = gpiod_chip_get_line(chip, pin);
    if (line == NULL || gpiod_line_request_output(line, "drace", 0) < 0) {
        fprintf(stderr, "GPIO %u indisponible\n", pin);
        return NULL;
    }
    return line;
}

static void led_set(struct gpiod_line *line, int value) {
    if (line != NULL)
        gpiod_line_set_value(line, value);
}

Car_t* car_init(Car_t *car) {
    car -> relative_speed   = 0.0;
    car -> direction        = 0.0;
    car -> gear             = 1;
    car -> absolute_speed   = 0.0;

    chip = gpiod_chip_open_by_name("gpiochip0");
    if (chip == NULL) {
        fprintf(stderr, "gpiochip0 inaccessible : LEDs desactivees\n");
    } else {
        led_gauche = led_request(LED_GAUCHE);
        led_droite = led_request(LED_DROITE);
        led_frein  = led_request(LED_FREIN);
        led_accel  = led_request(LED_ACCEL);
    }
    return car;
}

// Reception des données venant du serveur
// Différents actionners (JGX (left joystick), GG (gachette gauche), GD (gachette droite)))

void car_reception(Car_t *car, const char *actionner, float valeur) {
    if (strcmp(actionner, "JGX") == 0) {
        // Joystick gauche X (direction)
        if (valeur < -1.0) {
            valeur = -1.0; 
        }
        if (valeur > 1.0) {
            valeur = 1.0;
        }
        car->direction = valeur;

        if (car -> direction < -0.02) {
            led_set(led_gauche, 1);
            led_set(led_droite, 0);
        } else if (car -> direction > 0.02) {
            led_set(led_droite, 1);
            led_set(led_gauche, 0);
        } else {
            led_set(led_gauche, 0);
            led_set(led_droite, 0);
        }
    
    } else if (strcmp(actionner, "GG") == 0) {
        // Gachette gauche (frein)
        if (valeur < 0.0){
            valeur = 0.0;
        }
        if (valeur > 1.0) {
            valeur = 1.0;
        }
        car->relative_speed = -valeur; // Freinage 
        
        if (car->relative_speed < -0.7) {
            led_set(led_frein, 1);
        } else {
            led_set(led_frein, 0);
        }
    
    } else if (strcmp(actionner, "GD") == 0) {
        // Gachette droite (acceleration)
        if (valeur < 0.0) {
            valeur = 0.0;
        }
        if (valeur > 1.0) {
            valeur = 1.0;
        }
        car->relative_speed = valeur; // Acceleration
        
        if (car->relative_speed > 0.7) {
            led_set(led_accel, 1);
        } else {
            led_set(led_accel, 0);
        }
    }     
}

