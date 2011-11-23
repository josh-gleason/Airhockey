#ifndef __SRC_OPENGL_MANAGEMENT_DISPLAY_H__
#define __SRC_OPENGL_MANAGEMENT_DISPLAY_H__

#include "GL.h"

#include "../structures/Parameters.h"

#include <sstream>
#include <string>

using namespace std;

extern Parameters options;

void display_function( );

void init_menu();

void processMenuEvents( int option );
void processStateMenuEvents( int option );
void processShapeMenuEvents( int option );


#endif
