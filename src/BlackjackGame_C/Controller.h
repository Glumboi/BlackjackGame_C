#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <raylib.h>
#include <stdio.h>

#define CONTROLLER_A GAMEPAD_BUTTON_RIGHT_FACE_DOWN
#define CONTROLLER_B GAMEPAD_BUTTON_RIGHT_FACE_RIGHT
#define CONTROLLER_X GAMEPAD_BUTTON_RIGHT_FACE_LEFT
#define CONTROLLER_Y GAMEPAD_BUTTON_RIGHT_FACE_UP

#define CONTROLLER_CROSS_DOWN GAMEPAD_BUTTON_LEFT_FACE_DOWN
#define CONTROLLER_CROSS_UP GAMEPAD_BUTTON_LEFT_FACE_UP
#define CONTROLLER_CROSS_LEFT GAMEPAD_BUTTON_LEFT_FACE_LEFT
#define CONTROLLER_CROSS_RIGHT GAMEPAD_BUTTON_LEFT_FACE_RIGHT

#define CONTROLLER_PRESSED(code) \
	code == GetGamepadButtonPressed() 

#endif