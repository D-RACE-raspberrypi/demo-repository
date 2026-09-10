#include "car.h"
#include <stdio.h>
#include <string.h>

Car_t* car_init(Car_t *car) {
    car -> relative_speed   = 0.0;
    car -> direction        = 0.0;
    car -> gear             = 1;
    car -> absolute_speed   = 0.0;
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
        servo_control(car->direction);
    
    } else if (strcmp(actionner, "GG") == 0) {
        // Gachette gauche (frein)
        if (valeur < 0.0){
            valeur = 0.0;
        }
        if (valeur > 1.0) {
            valeur = 1.0;
        }
        car->relative_speed = -valeur; // Freinage 
        car->absolute_speed = car->relative_speed * car->gear / 4.0; // Mise à jour de la vitesse absolue
        moteur_control(car->absolute_speed);
    } else if (strcmp(actionner, "GD") == 0) {
        // Gachette droite (acceleration)
        if (valeur < 0.0) {
            valeur = 0.0;
        }
        if (valeur > 1.0) {
            valeur = 1.0;
        }
        car->relative_speed = valeur; // Acceleration
        car->absolute_speed = car->relative_speed * car->gear / 4.0; // Mise à jour de la vitesse absolue
        moteur_control(car->absolute_speed);
    } else if (strcmp(actionner, "CROIXP") == 0) {
        car->gear = 1;
        car->absolute_speed = car->relative_speed * car->gear / 4.0; // Mise à jour de la vitesse absolue
        moteur_control(car->absolute_speed);
    } else if (strcmp(actionner, "CARREP") == 0) {
        car->gear = 2;
        car->absolute_speed = car->relative_speed * car->gear / 4.0; // Mise à jour de la vitesse absolue
        moteur_control(car->absolute_speed);
    } else if (strcmp(actionner, "TRIANGLEP") == 0) {
        car->gear = 3;
        car->absolute_speed = car->relative_speed * car->gear / 4.0; // Mise à jour de la vitesse absolue
        moteur_control(car->absolute_speed);
    } else if (strcmp(actionner, "RONDP") == 0) {
        car->gear = 4;
        car->absolute_speed = car->relative_speed * car->gear / 4.0; // Mise à jour de la vitesse absolue
        moteur_control(car->absolute_speed);
    }
}

