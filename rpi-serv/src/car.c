#include "car.h"
#include <stdio.h>
#include <string.h>

Car_t* car_init(Car_t *car) {
    car -> relative_speed   = 0.0;
    car -> direction        = 0.0;
    car -> gear             = 1;
    car -> absolute_speed   = 0.5;  // Valeur initiale de la vitesse absolue (0.5 correspond à une vitesse neutre)
    car -> trigger_accel     = 0.0;
    car -> trigger_brake     = 0.0;
    return car;
}

// Reception des données venant du serveur
// Différents actionners (JGX (left joystick), GG (gachette gauche), GD (gachette droite)))

void car_update_absolute_speed(Car_t *car) {    // Mise à jour de la vitesse absolue en fonction de la vitesse relative et du rapport
    car->absolute_speed = 0.5f + (car->relative_speed * 0.5f * car->gear / 4.0f); // Mise à jour de la vitesse absolue
    moteur_control(car->absolute_speed);
}

void car_display_state(Car_t *car) {  // Affichage de l'état actuel de la voiture
    printf("\033[H\033[J");           // Efface l'écran avant d'afficher l'état

    printf("Etat de la voiture :\n");
    printf("  Vitesse relative : %0.3f\n", car->relative_speed);
    printf("  Direction : %0.3f\n", car->direction);
    printf("  Rapport de vitesse : %d\n", car->gear);
    printf("  Vitesse absolue : %0.3f\n", car->absolute_speed);
    printf("  Gachette accel : %0.3f\n", car->trigger_accel);
    printf("  Gachette brake : %0.3f\n", car->trigger_brake);
    fflush(stdout); // Assurez-vous que l'affichage est immédiatement visible
}

void car_reception(Car_t *car, const char *actionner, float valeur) {
    if (strcmp(actionner, "JGX") == 0) {
        // Joystick gauche X (direction)
        if (valeur < -1.0) {
            valeur = -1.0; 
        }
        if (valeur > 1.0) {
            valeur = 1.0;
        }
        car->direction = -valeur;
        // printf("CAR : direction %0.3f\n", car->direction);
        servo_control(car->direction);
        
    } else if (strcmp(actionner, "GD") == 0) {
        // Gachette droite (acceleration)
        if (valeur < 0.0) {
            valeur = 0.0;
        }
        if (valeur > 1.0) {
            valeur = 1.0;
        }
        car->trigger_accel = valeur; // Enregistre la valeur de la gachette pour l'accélération
        car->relative_speed = car->trigger_accel - car->trigger_brake; // Acceleration
        if (car->relative_speed > 1.0) {
            car->relative_speed = 1.0;
        }
        car_update_absolute_speed(car); // Mise à jour de la vitesse absolue
        // printf("CAR : Accelere a %0.3f\n", car->absolute_speed);
    } 
      else if (strcmp(actionner, "GG") == 0) {
        // Gachette gauche (braking)
        if (valeur < 0.0) {
            valeur = 0.0;
        }
        if (valeur > 1.0) {
            valeur = 1.0;
        }
        car->trigger_brake = valeur; // Enregistre la valeur de la gachette pour le freinage
        car->relative_speed = car->trigger_accel - car->trigger_brake; // Braking reduces relative speed
        if (car->relative_speed < -1.0) {
            car->relative_speed = -1.0;
        }
        car_update_absolute_speed(car); // Mise à jour de la vitesse absolue
        // printf("CAR : Recule a %0.3f\n", car->absolute_speed);
    } 
      else if (strcmp(actionner, "CroixP") == 0) {
        car->gear = 1;
        car_update_absolute_speed(car); // Mise à jour de la vitesse absolue
        // printf("CAR : Rapport %d\n", car->gear);
    } 
      else if (strcmp(actionner, "CarreP") == 0) {
        car->gear = 2;
        car_update_absolute_speed(car); // Mise à jour de la vitesse absolue
        // printf("CAR : Rapport %d\n", car->gear);
    } 
      else if (strcmp(actionner, "TriangleP") == 0) {
        car->gear = 3;
        car_update_absolute_speed(car); // Mise à jour de la vitesse absolue
        // printf("CAR : Rapport %d\n", car->gear);
    } 
      else if (strcmp(actionner, "RondP") == 0) {
        car->gear = 4;
        car_update_absolute_speed(car); // Mise à jour de la vitesse absolue
        // printf("CAR : Rapport %d\n", car->gear);
    } 
      else {
        // Actionneur inconnu
        return;
    }
    car_display_state(car); // Affiche l'état actuel de la voiture après la réception des données
    return;
}

