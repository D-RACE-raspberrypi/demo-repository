#include "includes/controller.h"

// Broches physiques 11, 13, 15, 19 pour les LEDs
#define LED_CROIX 17
#define LED_ROND 27
#define LED_CARRE 22
#define LED_TRIANGLE 10



int main(int argc, char *argv[]) {

    // Initialisation de la puce GPIO
    struct gpiod_chip *chip = gpiod_chip_open_by_name("gpiochip0");
    if (!chip) {
        fprintf(stderr, "Impossible d'ouvrir la puce GPIO\n");
        return EXIT_FAILURE;
    }

    // Création du tableau de pointeurs
    struct gpiod_line *leds[4];

    leds[0] = gpiod_chip_get_line(chip, LED_CROIX);
    leds[1] = gpiod_chip_get_line(chip, LED_ROND);
    leds[2] = gpiod_chip_get_line(chip, LED_CARRE);
    leds[3] = gpiod_chip_get_line(chip, LED_TRIANGLE);

    for (int i=0; i<4; i++) {
        if (!leds[i] || gpiod_line_request_output(leds[i], "led_control", 0) < 0) {
            fprintf(stderr, "Impossible d'obtenir la ligne GPIO pour la LED %d\n", i);
            gpiod_chip_close(chip);
            return EXIT_FAILURE;
        }
    }


    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_GAMECONTROLLER) < 0) {
        printf("Erreur d'initialisation de SDL : %s\n", SDL_GetError());
        return -1;
    }
    
    printf("Recherche de manettes...\n");
    
    // Vérifie si une manette est connectée
    if (SDL_NumJoysticks() < 1) {
        printf("Aucune manette détectée.\n");
        SDL_Quit();
        return -1;
    }
    // Ouvre la  manette
    SDL_GameController *controller = SDL_GameControllerOpen(0);
    if (controller == NULL) {
        printf("Impossible d'ouvrir la manette : %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    printf("Manette detectée : %s\n", SDL_GameControllerName(controller));
    
    SDL_GameControllerRumble(controller, 0.5, 1, 500); // Fait vibrer la manette
    SDL_GameControllerSetLED(controller, 0x00, 0xFF, 0x00); // Allume la LED de la manette en vert
    
    // Boucle principale
    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_CONTROLLERAXISMOTION: // Mouvement des joystick
                joystick(event);
                break;
            
            case SDL_CONTROLLERBUTTONDOWN: // Boutons appuyés
                // press_button(event);
                led_boutons(press_button(event), leds); // Met à jour les LEDs en fonction du bouton pressé
                break;
            
            case SDL_CONTROLLERBUTTONUP: // Boutons relâchés
                // release_button(event);
                led_boutons(release_button(event), leds); // Met à jour les LEDs en fonction du bouton relâché
                break;
            
            case SDL_QUIT: // Quitte l'application
                printf("SDL_QUIT trigger\n");
                running = 0;
                break;
            }
        }
    }
    
    // Fermeture et nettoyage
    printf("sortie du programme\n");

    for (int i = 0; i < 4; i++) {
        gpiod_line_set_value(leds[i], 0); // Sécurité : on éteint la LED
        gpiod_line_release(leds[i]);      // Libération de la broche
    }
    gpiod_chip_close(chip);
    
    SDL_GameControllerClose(controller);
    SDL_Quit();
    return 0;
}