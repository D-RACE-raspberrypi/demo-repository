#include "includes/controller.h"
#include <gpiod.h>
#include <string.h>
#include <stdlib.h>


int led_boutons(const char *data_to_esp, struct gpiod_line **led_lines) {
    // led_lines[0] = LED Croix
    // led_lines[1] = LED Rond
    // led_lines[2] = LED Carré
    // led_lines[3] = LED Triangle

    if (strcmp(data_to_esp, "CroixP\n") == 0) {
        gpiod_line_set_value(led_lines[0], 1); // Allume la LED Croix
    } else if (strcmp(data_to_esp, "CroixR\n") == 0) {
        gpiod_line_set_value(led_lines[0], 0); // Éteint la LED Croix
    } 
      else if (strcmp(data_to_esp, "RondP\n") == 0) {
        gpiod_line_set_value(led_lines[1], 1); // Allume la LED Rond
    } else if (strcmp(data_to_esp, "RondR\n") == 0) {
        gpiod_line_set_value(led_lines[1], 0); // Éteint la LED Rond
    } 
      else if (strcmp(data_to_esp, "CarreP\n") == 0) {
        gpiod_line_set_value(led_lines[2], 1); // Allume la LED Carré
    } else if (strcmp(data_to_esp, "CarreR\n") == 0) {
        gpiod_line_set_value(led_lines[2], 0); // Éteint la LED Carré
    } 
      else if (strcmp(data_to_esp, "TriangleP\n") == 0) {
        gpiod_line_set_value(led_lines[3], 1); // Allume la LED Triangle
    } else if (strcmp(data_to_esp, "TriangleR\n") == 0) {
        gpiod_line_set_value(led_lines[3], 0); // Éteint la LED Triangle
    } 
      else {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}