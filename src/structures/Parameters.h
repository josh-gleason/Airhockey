/**
  * @file Paramters.h
  * Parameter container for Air Hockey
  *
  * @author Marvin Smith
  * @author Joshua Gleason
*/
#ifndef __SRC_STRUCTURES_PARAMETERS_H__
#define __SRC_STRUCTURES_PARAMETERS_H__

// ../opengl
#include "GL.h"

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>

// ./
#include "Object.h"
#include "Camera.h"
#include "Light.h"
#include "Physics.h"

// ../math/
#include "vector.h"
#include "mat.h"

// bullet physics engine
#include <btBulletDynamicsCommon.h>

using namespace std;

//typedef boost::shared_ptr<Shape> ShapePtr;

/**
   @class Parameters
   @brief Storage for program options

   This class stores everything for the OpenGL Program

*/
class Parameters{

   public:

      enum PaddleType {ROUND, TRIANGLE, SQUARE};

      /**
        * Parameter Constructor
        * 
        * builds the parameter class and sets the mode
        * lock
      */
      Parameters();
      /**
        * Destructor
      */
      ~Parameters();
     
      /**
        * Print parameter values in order to see program configuration
      */
      void print();
      
      /**
        * construct the shapes and initialize them in OpenGL
      */
      void construct_objects( );
      
      void rotate();
      /** Camera Object */
      //Camera  camera;
      
      /** Menu Object  */
      //int menu;
   
      /** Mouse is down command */
      bool mouse_down;

      /** Anchor for shape initial is set */
      //int mouse_anchor;

      int x_beg, y_beg, x_cur, y_cur;

      GLfloat aspect;

      string filename;

      GLuint program;

      GLuint model_view;
      GLuint projection;

      point4 eye;
      point4 center;
      point4 tcenter;
      vec4 up;

      GLfloat fova;
      GLfloat ratio;
      GLfloat left;
      GLfloat right;
      GLfloat top;
      GLfloat bottom;
      GLfloat zNear;
      GLfloat zFar;

      mat4 projectionMatrix;

      CameraMode cam_mode;
      GLfloat cam_lookStep;
      GLfloat cam_moveStep;
      unsigned int cam_timerStep;

      Object* board;
      Object* paddle1;
      Object* paddle2;
      Object* puck;

      GLfloat pdl_moveStep;
      GLfloat pdl_timerStep;
      btScalar pdl1_maxVelocity;
      btScalar pdl2_maxVelocity;

      // the destination that the paddles attempt to move to in (x,z) coords
      vec2 paddle1_dest;
      vec2 paddle2_dest;

      void set_paddle1_dest(vec2 dest);
      void set_paddle2_dest(vec2 dest);

      Camera camera;
      Light light;
      Physics_Model physics;
      unsigned int light_timerStep;
      GLfloat light_moveStep;

      int glut_window_x;
      int glut_window_y;
      int glut_initial_x;
      int glut_initial_y;
      string glut_window_name;

      int p1_score;
      int p2_score;

      // player 1 is the AI player right now
      bool ai_enabled;
      float ai_aggressive; // TODO not working
      float ai_difficulty;
};

#endif
