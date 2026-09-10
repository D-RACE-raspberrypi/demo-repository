#ifndef CAR_H
#define CAR_H
#include "servo.h"
#include "moteur.h"
// Numeros de broche BCM des LEDs

typedef struct Car_t {
    float relative_speed;   // 0.0 to 1.0
    float direction;        // -1.0 (left) to 1.0 (right)
    int gear;               // 1, 2, 3, 4   (0.0 -> 0.25 -> 0.50 -> 0.75 -> 1.0)
    float absolute_speed;   // Calculated as relative_speed * 0.25 * gear
} Car_t;

Car_t* car_init(Car_t *car);

void car_reception(Car_t *car, const char *actionner, float valeur);

#endif // CAR_H