#include "Parameters.h"

Parameters::Parameters( ){

   //Mouse anchor set to zero
   //mouse_anchor = 0;
   eye    = vec4( 0.0, 0.0, -1.0, 1.0 );
   center = vec4( 0.0, 0.0,  0.0, 1.0);
   up     = vec4( 0.0, 1.0,  0.0, 1.0);

   fova     = 20.0;  ratio = 1.0;
   left     = -2.0;  right = 2.0;
   bottom   = -2.0;  top = 2.0;
   zNear    = 0.25;  zFar = 5.0;

//   cam_mode = CAMERA_Y_LOCK_BOTH;
   cam_mode = CAMERA_Y_LOCK_VERT;
//   cam_mode = CAMERA_FREE;

   cam_lookStep = 1.0;
   cam_moveStep = 0.05;
   cam_timerStep = 10;

   pdl_moveStep = 0.1;
   pdl_timerStep = 17;  // ~60 Hz

   // in units/sec (this is the to restrict the mouse movement to the
   // speed of the paddle)
   pdl_maxVelocity = sqrt(pdl_moveStep*pdl_moveStep*7200.);

   mouse_down = false;

   projectionMatrix = Perspective(fova, ratio, zNear, zFar);

   board = NULL;
   paddle1 = NULL;
   paddle2 = NULL;
   puck = NULL;

   p1_score = 0;
   p2_score = 0;
}

Parameters::~Parameters(){

   if(board != NULL)
      delete board;
   if(paddle1 != NULL)
      delete paddle1;
   if(paddle2 != NULL)
      delete paddle2;
   if(puck != NULL)
      delete puck;
}

void Parameters::construct_objects( ){
   
   //box.init( program );
}

void Parameters::rotate(  ){

   double dist = vdistance( tcenter, eye);
   vec4 norm = tcenter - eye;

   double r     = dist;
   double phi   = asin(norm.y/r);
   double theta = atan2(norm.z,norm.x);

   double addition;
   if( fabs(x_cur-x_beg) > 0 ){
      addition = ((x_cur-x_beg)*3.141529/180.0)*0.1;
      theta = theta + addition*.1;
   }
   if( fabs(y_cur-y_beg) > 0 ){
      addition = ((y_cur-y_beg)*3.141529/180.0)*0.1;
      phi = phi + addition*.1;
   }
   if(fabs(phi) > (3.141529/2.0)) phi = (phi/fabs(phi))*3.141529/2.0;

   vec4 result( r * cos(phi)*cos(theta), r*sin(phi), r * cos(phi)*sin(theta),0.0); 

   center = result + eye;
   center.w = 1;
}

