#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int release_button(SDL_Event event);
int press_button(SDL_Event event);
int joystick(SDL_Event event);

#ifndef CONTROLLER_H
#define CONTROLLER_H

/**
 * @brief Handles the release of buttons on an SDL controller and printfs corresponding data to an ESP device.
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
int release_button(SDL_Event event);

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
int press_button(SDL_Event event);

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
int joystick(SDL_Event event);

#endif // CONTROLLER_H
