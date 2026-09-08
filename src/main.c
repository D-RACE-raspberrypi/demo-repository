#include <includes/controller.h>

int main(int argc, char *argv[]) {
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
    printf("Manette detecter : %s\n", SDL_GameControllerName(controller));
    
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
                press_button(event);
                break;
            
            case SDL_CONTROLLERBUTTONUP: // Boutons relâchés
                release_button(event);
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
    SDL_GameControllerClose(controller);
    SDL_Quit();
    return 0;
}