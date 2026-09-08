#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
// gcc -o core.exe core_X.x.c -lSDL2
// sudo ./core.exe


/**
 * @brief Handles button press events from an SDL controller and printfs corresponding data to an ESP device.
 * 
 * The function supports the following buttons:
 * - SDL_CONTROLLER_BUTTON_A: Printfs "CroixP\n" (Cross button pressed).
 * - SDL_CONTROLLER_BUTTON_B: Printfs "RondP\n" (Circle button pressed).
 * - SDL_CONTROLLER_BUTTON_X: Printfs "CarreP\n" (Square button pressed).
 * - SDL_CONTROLLER_BUTTON_Y: Printfs "TriangleP\n" (Triangle button pressed).
 * - SDL_CONTROLLER_BUTTON_BACK: Printfs "SelectP\n" (Select button pressed).
 * - SDL_CONTROLLER_BUTTON_START: Printfs "StartP\n" (Start button pressed).
 * - SDL_CONTROLLER_BUTTON_LEFTSHOULDER: Printfs "L1P\n" (L1 button pressed).
 * - SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: Printfs "R1P\n" (R1 button pressed).
 * - SDL_CONTROLLER_BUTTON_LEFTSTICK: Printfs "L3P\n" (L3 button pressed).
 * - SDL_CONTROLLER_BUTTON_RIGHTSTICK: Printfs "R3P\n" (R3 button pressed).
 * - SDL_CONTROLLER_BUTTON_DPAD_UP: Printfs "HautP\n" (D-pad up pressed).
 * - SDL_CONTROLLER_BUTTON_DPAD_DOWN: Printfs "BasP\n" (D-pad down pressed).
 * - SDL_CONTROLLER_BUTTON_DPAD_LEFT: Printfs "GaucheP\n" (D-pad left pressed).
 * - SDL_CONTROLLER_BUTTON_DPAD_RIGHT: Printfs "DroitP\n" (D-pad right pressed).
 * - SDL_CONTROLLER_BUTTON_GUIDE: Printfs "PsP\n" (PS button pressed).
 * - SDL_CONTROLLER_BUTTON_MISC1: Printfs "TouchpadP\n" (Touchpad button pressed).
 * 
 * If the PS button (SDL_CONTROLLER_BUTTON_GUIDE) is pressed, the application is stopped.
 * 
 * @note The function uses the `printf` function to transmit data over a socket.
 *       Ensure the `sock` variable is properly initialized and connected before calling this function.
 */
int press_button(SDL_Event event) {
    char *data_to_esp = "";
    switch (event.cbutton.button){
        case SDL_CONTROLLER_BUTTON_A : // Bouton Croix appuyé
            data_to_esp = "CroixP\n";
            break;
        case SDL_CONTROLLER_BUTTON_B : // Bouton Rond appuyé
            data_to_esp = "RondP\n";
            break;
        case SDL_CONTROLLER_BUTTON_X : // Bouton Carré appuyé
            data_to_esp = "CarreP\n";
            break;
        case SDL_CONTROLLER_BUTTON_Y : // Bouton Triangle appuyé
            data_to_esp = "TriangleP\n";
            break;
        case SDL_CONTROLLER_BUTTON_BACK : // Bouton Select appuyé
            data_to_esp = "SelectP\n";
            break;
        case SDL_CONTROLLER_BUTTON_START : // Bouton Start appuyé
            data_to_esp = "StartP\n";
            break;
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER : // Bouton L1 appuyé
            data_to_esp = "L1P\n";
            break;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER : // Bouton R1 appuyé
            data_to_esp = "R1P\n";
            break;
        case SDL_CONTROLLER_BUTTON_LEFTSTICK : // Bouton L3 appuyé
            data_to_esp = "L3P\n";
            break;
        case SDL_CONTROLLER_BUTTON_RIGHTSTICK : // Bouton R3 appuyé
            data_to_esp = "R3P\n";
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_UP : // Bouton haut appuyé
            data_to_esp = "HautP\n";
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN : // Bouton bas appuyé
            data_to_esp = "BasP\n";
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT : // Bouton gauche appuyé
            data_to_esp = "GaucheP\n";
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT : // Bouton droit appuyé
            data_to_esp = "DroitP\n";
            break;
        case SDL_CONTROLLER_BUTTON_GUIDE : // Bouton PS appuyé
            data_to_esp = "PsP\n";
            system("exit");
            break;
            // Stoppe l'application
        case SDL_CONTROLLER_BUTTON_MISC1 : // Bouton Touchpad appuyé
            data_to_esp = "TouchpadP\n";
            break;
        
        default:
            return EXIT_FAILURE;
        }
        printf("%s", data_to_esp);
        return EXIT_SUCCESS;
}

/**
 * @brief Handles joystick events and processes input from the controller's axes.
 * 
 * This function processes SDL joystick events and maps the input values from 
 * the controller's axes to corresponding variables. It also printfs the processed 
 * values to a specified socket using the `printfFloat` function.
 * 
 * @param event (type: SDL_Event) The SDL event containing joystick axis data.
 * @param sock (int) The socket descriptor used for printfing the data.
 * 
 * The function handles the following axes:
 * - SDL_CONTROLLER_AXIS_LEFTX: Left joystick horizontal axis.
 * - SDL_CONTROLLER_AXIS_LEFTY: Left joystick vertical axis.
 * - SDL_CONTROLLER_AXIS_RIGHTX: Right joystick horizontal axis.
 * - SDL_CONTROLLER_AXIS_RIGHTY: Right joystick vertical axis.
 * - SDL_CONTROLLER_AXIS_TRIGGERLEFT: Left trigger axis.
 * - SDL_CONTROLLER_AXIS_TRIGGERRIGHT: Right trigger axis.
 * 
 * For each axis, the function:
 * - Converts the axis value to a float.
 * - Printfs the value to a socket with a specific identifier using `printfFloat`.
 * - Updates the corresponding variable with the new value.
 * 
 * If the axis does not match any of the handled cases, the function does nothing.
 */
int joystick(SDL_Event event) {
    float left_joy_x_value = 0.0;
    float left_joy_y_value = 0.0;
    float right_joy_x_value = 0.0;
    float right_joy_y_value = 0.0;
    float press_L2_value = 0.0;
    float press_R2_value = 0.0;
    float value_LX, value_LY, value_RX, value_RY, value_L2, value_R2;
    
    switch (event.caxis.axis){
    case SDL_CONTROLLER_AXIS_LEFTX: // Joystick gauche X
        value_LX = (float)(event.caxis.value);
        printf("JGX: %f\n", value_LX);
        break;
    case SDL_CONTROLLER_AXIS_LEFTY: // Joystick gauche Y
        value_LY = (float)(event.caxis.value);
        printf("JGY: %f\n", value_LY);
        break;
    case SDL_CONTROLLER_AXIS_RIGHTX: // Joystick droit X
        value_RX = (float)(event.caxis.value);
        printf("JDX: %f\n", value_RX);
        break;
    case SDL_CONTROLLER_AXIS_RIGHTY: // Joystick droit Y
        value_RY = (float)(event.caxis.value);
        printf("JDY: %f\n", value_RY);
        break;
    case SDL_CONTROLLER_AXIS_TRIGGERLEFT: // Gâchette gauche
        value_L2 = (float)(event.caxis.value);
        printf("GG: %f\n", value_L2);
        press_L2_value = value_L2;
        break;
    case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: // Gachette droite
        value_R2 = (float)(event.caxis.value);
        printf("GD: %f\n", value_R2);
        press_R2_value = value_R2;
        break;
    default:
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief Handles the release of buttons on an SDL controller and printfs corresponding data to an ESP device.
 *
 * This function processes SDL controller button release events and printfs a specific string
 * to the ESP device over a socket connection, indicating which button was released.
 *
 * @param event (type: SDL_Event) The SDL event containing information about the button release.
 * @param sock (int) The socket descriptor used for printfing the data.
 * 
 * The function supports the following buttons:
 * - SDL_CONTROLLER_BUTTON_A: Printfs "CroixR\n" for the A button release.
 * - SDL_CONTROLLER_BUTTON_B: Printfs "RondR\n" for the B button release.
 * - SDL_CONTROLLER_BUTTON_X: Printfs "CarreR\n" for the X button release.
 * - SDL_CONTROLLER_BUTTON_Y: Printfs "TriangleR\n" for the Y button release.
 * - SDL_CONTROLLER_BUTTON_BACK: Printfs "SelectR\n" for the Back button release.
 * - SDL_CONTROLLER_BUTTON_START: Printfs "StartR\n" for the Start button release.
 * - SDL_CONTROLLER_BUTTON_LEFTSHOULDER: Printfs "L1R\n" for the Left Shoulder button release.
 * - SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: Printfs "R1R\n" for the Right Shoulder button release.
 * - SDL_CONTROLLER_BUTTON_LEFTSTICK: Printfs "L3R\n" for the Left Stick button release.
 * - SDL_CONTROLLER_BUTTON_RIGHTSTICK: Printfs "R3R\n" for the Right Stick button release.
 * - SDL_CONTROLLER_BUTTON_DPAD_UP: Printfs "HautR\n" for the D-Pad Up button release.
 * - SDL_CONTROLLER_BUTTON_DPAD_DOWN: Printfs "BasR\n" for the D-Pad Down button release.
 * - SDL_CONTROLLER_BUTTON_DPAD_LEFT: Printfs "GaucheR\n" for the D-Pad Left button release.
 * - SDL_CONTROLLER_BUTTON_DPAD_RIGHT: Printfs "DroiteR\n" for the D-Pad Right button release.
 * - SDL_CONTROLLER_BUTTON_GUIDE: Printfs "PsR\n" for the Guide button release.
 * - SDL_CONTROLLER_BUTTON_MISC1: Printfs "TouchpadR\n" for the Touchpad button release.
 *
 * If the button is not recognized, the function does nothing.
 *
 * @note The function uses the `printf` function to transmit data over a socket.
 *       Ensure the socket (`sock`) and the `data_to_esp` variable are properly initialized
 *       before calling this function.
 */
int release_button(SDL_Event event) {
    char *data_to_esp = "";
    switch (event.cbutton.button){
        case SDL_CONTROLLER_BUTTON_A : // Bouton Croix relâché
            data_to_esp = "CroixR\n";
            break;
        case SDL_CONTROLLER_BUTTON_B : // Bouton Rond relâché
            data_to_esp = "RondR\n";
            break;
        case SDL_CONTROLLER_BUTTON_X : // Bouton Carré relâché
            data_to_esp = "CarreR\n";
            break;
        case SDL_CONTROLLER_BUTTON_Y : // Bouton Triangle relâché
            data_to_esp = "TriangleR\n";
            break;
        case SDL_CONTROLLER_BUTTON_BACK : // Bouton Select relâché
            data_to_esp = "SelectR\n";
            break;
        case SDL_CONTROLLER_BUTTON_START : // Bouton Start relâché
            data_to_esp = "StartR\n";
            break;
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER : // Bouton L1 relâché
            data_to_esp = "L1R\n";
            break;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER : // Bouton R1 relâché
            data_to_esp = "R1R\n";
            break;
        case SDL_CONTROLLER_BUTTON_LEFTSTICK : // Bouton L3 relâché
            data_to_esp = "L3R\n";
            break;
        case SDL_CONTROLLER_BUTTON_RIGHTSTICK : // Bouton R3 relâché
            data_to_esp = "R3R\n";
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_UP : // Bouton haut relâché
            data_to_esp = "HautR\n";
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN : // Bouton bas relâché
            data_to_esp = "BasR\n";
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT : // Bouton gauche relâché
            data_to_esp = "GaucheR\n";
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT : // Bouton droit relâché
            data_to_esp = "DroiteR\n";
            break;
        case SDL_CONTROLLER_BUTTON_GUIDE : // Bouton PS relâché
            data_to_esp = "PsR\n";
            
            // close(sock);
            // exit(0);break;
        case SDL_CONTROLLER_BUTTON_MISC1 : // Bouton Touchpad relâché
            data_to_esp = "TouchpadR\n";
            break;
        default:
            return EXIT_FAILURE;
        }
        printf("%s", data_to_esp);
        return EXIT_SUCCESS;
}

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
    printf("Manette detecter : %s\nConnexion à l'ESP32...\n", SDL_GameControllerName(controller));
    
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