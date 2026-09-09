#include <includes/controller.h>

char* press_button(SDL_Event event) {
    const char *data_to_esp = "";
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
            break;
            // Stoppe l'application
        case SDL_CONTROLLER_BUTTON_MISC1 : // Bouton Touchpad appuyé
            data_to_esp = "TouchpadP\n";
            break;
        
        default:
            return "";
        }
        printf("%s", data_to_esp);
        return data_to_esp;
}

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
        value_LX = (float)(event.caxis.value/32767.0);
        printf("JGX: %f\n", value_LX);
        break;
    case SDL_CONTROLLER_AXIS_LEFTY: // Joystick gauche Y
        value_LY = (float)(event.caxis.value/32767.0);
        printf("JGY: %f\n", value_LY);
        break;
    case SDL_CONTROLLER_AXIS_RIGHTX: // Joystick droit X
        value_RX = (float)(event.caxis.value/32767.0);
        printf("JDX: %f\n", value_RX);
        break;
    case SDL_CONTROLLER_AXIS_RIGHTY: // Joystick droit Y
        value_RY = (float)(event.caxis.value/32767.0);
        printf("JDY: %f\n", value_RY);
        break;
    case SDL_CONTROLLER_AXIS_TRIGGERLEFT: // Gâchette gauche
        value_L2 = (float)(event.caxis.value/32767.0);
        printf("GG: %f\n", value_L2);
        press_L2_value = value_L2;
        break;
    case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: // Gachette droite
        value_R2 = (float)(event.caxis.value/32767.0);
        printf("GD: %f\n", value_R2);
        press_R2_value = value_R2;
        break;
    default:
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


char* release_button(SDL_Event event) {
    const char *data_to_esp = "";
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
            exit(0);
            break;
        case SDL_CONTROLLER_BUTTON_MISC1 : // Bouton Touchpad relâché
            data_to_esp = "TouchpadR\n";
            break;
        default:
            return "";
        }
        printf("%s", data_to_esp);
        return data_to_esp;
}