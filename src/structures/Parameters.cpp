#include "Parameters.h"

Parameters::Parameters( ){

   winner = NULL;
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

   // 0.15 might be a little fast lol
   pdl_moveStep = 0.1;
   pdl_timerStep = 17;  // ~60 Hz

   // in units/sec (mostly this is the to restrict the mouse movement to the
   // max speed of the paddle) negative value disables maximum (unwise)
   // 0.2 - easy
   // 0.3 - medium
   // 0.4 - hard
   // 0.5 - insane
   // > 0.5 - impossible
   ai_difficulty = 0.2;

   pdl1_maxVelocity = sqrt(pdl_moveStep*pdl_moveStep*7200.);
   pdl2_maxVelocity = sqrt(pdl_moveStep*pdl_moveStep*7200.);

   mouse_down = false;

   projectionMatrix = Perspective(fova, ratio, zNear, zFar);

   board = NULL;
   puck = NULL;
   round1 = NULL;
   round2 = NULL;
   tri1 = NULL;
   tri2 = NULL;
   sqr1 = NULL;
   sqr2 = NULL;

   p1_score = 0;
   p2_score = 0;

   // should start false then be changed using enabledAI() or disableAI()
   ai_enabled = false;

   // range [0,1]
   ai_aggressive = 0.5;
}

Parameters::~Parameters(){

   if(board != NULL)
      delete board;
   if(puck != NULL)
      delete puck;
   if(round1 != NULL)
      delete round1;
   if(round2 != NULL)
      delete round2;
   if(tri1 != NULL)
      delete tri1;
   if(tri2 != NULL)
      delete tri2;
   if(sqr1 != NULL)
      delete sqr1;
   if(sqr2 != NULL)
      delete sqr2;
}

void Parameters::construct_objects( ){
   
   //box.init( program );
}

void Parameters::set_paddle1_dest(vec2 dest){
   // simple bounding rectangle (hard coded)
   dest.x = max(-3.1188105f,dest.x);
   dest.x = min(-0.2f,dest.x);

   // actually z
   dest.y = min(1.749742f,dest.y);
   dest.y = max(-1.749742f,dest.y);

   paddle1_dest = dest;
}

void Parameters::set_paddle2_dest(vec2 dest){
   // simple bounding rectangle (hard coded)
   dest.x = min(3.1188105f,dest.x);
   dest.x = max(0.2f,dest.x);

   // actually z
   dest.y = min(1.749742f,dest.y);
   dest.y = max(-1.749742f,dest.y);

   paddle2_dest = dest;
}

void Parameters::set_paddle_shape( PaddleType shape )
{
   if ( shape == ROUND ) {
      paddle1 = round1;
      paddle2 = round2;

      //paddle1Shape

   } else if ( shape == TRIANGLE ) {
      paddle1 = tri1;
      paddle2 = tri2;
   } else if ( shape == SQUARE ) {
      paddle1 = sqr1;
      paddle2 = sqr2;
   }

   // TODO: change some other things here
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

