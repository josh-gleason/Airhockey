#include "display.h"

void display_function( void )
{
   string name;
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   if( options.view_mode == 0 || options.view_mode == 4){
      glClearColor( 0.5, 0.5, 0.5, 1.0 );
      
      //glUniform3fv( theta, 1, Theta );
      mat4 ident;
      options.board->draw_shape(   options.camera.getTransform(), options.projectionMatrix, options.light.m_position);
      stringstream sin;
      ///player 2 board
      sin.clear();
      sin.str("");
      sin << options.p2_score;
      string p2;
      sin >> p2;
      glPushMatrix();
      glRasterPos3f( 3.58,.2, 0.1);
      glColor3f(1,1,1);
      name = "Player 2 Score: ";
//      for(size_t i=0; i<name.size(); i++)
//         glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24 , name[i] );
//      for(size_t i=0; i<p2.size(); i++)
//         glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24 , p2[i] );
      glPopMatrix();

      ///player1 board
      sin.clear();
      sin.str("");
      sin << options.p1_score;
      string p1;
      sin >> p1;
      glPushMatrix();
      glRasterPos3f(-3.58,.2, 0.1);
      glColor3f(1,1,1);
      name = "Player 1 Score: ";
//      for(size_t i=0; i<name.size(); i++)
//         glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24 , name[i] );
//      for(size_t i=0; i<p1.size(); i++)
//         glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24 , p1[i] );
      glPopMatrix();


      options.puck->draw_shape(    options.camera.getTransform(), options.projectionMatrix, options.light.m_position);
      options.paddle1->draw_shape( options.camera.getTransform(), options.projectionMatrix, options.light.m_position);
      options.paddle2->draw_shape( options.camera.getTransform(), options.projectionMatrix, options.light.m_position);
      options.hud.draw_shape();
   }
   else if( options.view_mode == 1 ){
      
      glClearColor( 0.9, 0.4, 0.9, 1.0 );
      options.scoreboard.draw_shape();


   }
   else if( options.view_mode == 2 ){
      glClearColor( 1.0, 1.0, 1.0, 1.0);
      options.winner->draw_shape();
   }

   glutSwapBuffers();

}


void ReSizeGLScene(int Width, int Height)
{

   glViewport( 0, 0, Width, Height);

   GLfloat aspect = GLfloat(Width)/Height;

   options.aspect = aspect;
   options.glut_window_x = Width;
   options.glut_window_y = Height;

   //options.camera.set_aspect_ratio( aspect);
}

void init_menu( ){
   
   options.state_submenu = glutCreateMenu( &processStateMenuEvents);
   glutAddMenuEntry("Pause",0);
   glutAddMenuEntry("Reset",1);
   glutAddMenuEntry("Resume",2);

   options.shape_submenu = glutCreateMenu( &processShapeMenuEvents);
   glutAddMenuEntry("Round",0);
   glutAddMenuEntry("Square",1);
   //glutAddMenuEntry("Wedge",2); // TODO

   options.ai_submenu = glutCreateMenu( &processAIMenuEvents);
   glutAddMenuEntry("AI On", 0);
   glutAddMenuEntry("AI Off",1);

   options.difficulty_submenu = glutCreateMenu( &processDiffMenuEvents);
   glutAddMenuEntry("God", 0);
   glutAddMenuEntry("Tough",1);
   glutAddMenuEntry("Okay",2);
   glutAddMenuEntry("I'm a wimp",3);
   
   options.menu = glutCreateMenu( &processMenuEvents );
   glutAddSubMenu("Game State",options.state_submenu);
   glutAddSubMenu("Paddle Shape",options.shape_submenu);
   glutAddSubMenu("AI Mode",options.ai_submenu);
   glutAddSubMenu("Difficulty",options.difficulty_submenu);
   glutAddMenuEntry("Hall of Fame",1);
   
   glutAddMenuEntry("Quit Simulation",0);
   glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void processMenuEvents( int option ){

   /*********************************************************/
   /*    If not picking mode, then unset pick mode flag     */
   /*********************************************************/
   switch( option ){

      /**************************************/
      /*             Quit Program           */
      /**************************************/
      case 0: exit(0);
              break;

      /*******************************/
      /*        HALL OF FAME         */
      /*******************************/
      case 1:
         if(options.view_mode == 1)
            options.view_mode = 0;
         else
            options.view_mode = 1;
         break;

   }
}

void processStateMenuEvents( int option ){
      
   vec3 motion;

   switch( option ){

      case 0:
         options.view_mode = 4;
         break;
      case 1:
         //reset score
         options.p1_score = 0;
         options.p2_score = 0;
         options.hud.p1_score(options.p1_score);
         options.hud.p2_score(options.p2_score);

         //reset puck and paddle position
         options.physics.puckRigidBody->setWorldTransform(btTransform(btQuaternion(1,0,0,0),btVector3(0, 0, 0)));
         options.physics.paddle1RigidBody->setWorldTransform(btTransform(btQuaternion(1,0,0,0),btVector3(-3, 0, 0)));
         options.physics.paddle2RigidBody->setWorldTransform(btTransform(btQuaternion(1,0,0,0),btVector3( 3, 0, 0)));

         options.set_paddle1_dest(vec2(-3.0, 0.0));
         options.set_paddle2_dest(vec2( 3.0, 0.0));

         /*     Start Puck     */
         //get an update of the motion state
         options.physics.puckRigidBody->getMotionState()->getWorldTransform( options.physics.puck_trans );

         //load new position into structure for paddle1 model
         motion = vec3( options.physics.paddle1_trans.getOrigin().getX(), options.physics.paddle1_trans.getOrigin().getY(), options.physics.paddle1_trans.getOrigin().getZ());

         //push new position into paddle1
         options.puck->set_translation( motion );
         /*   End puck    */

         /*    Start paddle 1    */
         //get an update of the motion state
         options.physics.paddle1RigidBody->getMotionState()->getWorldTransform( options.physics.paddle1_trans );

         //load new position into structure for paddle1 model
         motion = vec3( options.physics.paddle1_trans.getOrigin().getX(), options.physics.paddle1_trans.getOrigin().getY(), options.physics.paddle1_trans.getOrigin().getZ());

         //push new position into paddle1
         options.paddle1->set_translation( motion );
         /*    End paddle 1   */
         /*    Start paddle 2    */
         //get an update of the motion state
         options.physics.paddle2RigidBody->getMotionState()->getWorldTransform( options.physics.paddle2_trans );

         //load new position into structure for paddle2 model
         motion = vec3( options.physics.paddle2_trans.getOrigin().getX(), options.physics.paddle2_trans.getOrigin().getY(), options.physics.paddle2_trans.getOrigin().getZ());

         //push new position into paddle2
         options.paddle2->set_translation( motion );
         /*    End paddle 2   */

         // reset velocities
         options.physics.paddle1RigidBody->setLinearVelocity(  btVector3(0.0,0.0,0.0));
         options.physics.paddle1RigidBody->setAngularVelocity( btVector3(0.0,0.0,0.0));

         options.physics.paddle2RigidBody->setLinearVelocity(  btVector3(0.0,0.0,0.0));
         options.physics.paddle2RigidBody->setAngularVelocity( btVector3(0.0,0.0,0.0));

         options.physics.puckRigidBody->setLinearVelocity(  btVector3(0.0,0.0,0.0));
         options.physics.puckRigidBody->setAngularVelocity( btVector3(0.0,0.0,0.0));


         break;
      case 2:
         options.view_mode = 0; 
         break;

   }
}

void processShapeMenuEvents( int option ){

   switch( option ){

      case 0: 
         options.set_paddle_shape(Parameters::ROUND);
         break;
      case 1:
         options.set_paddle_shape(Parameters::SQUARE);
         break;
      case 2:
         options.set_paddle_shape(Parameters::TRIANGLE);
         break;
   }
}

void processAIMenuEvents( int option ){

   switch( option ){

      case 0: 
         enableAI();
         break;
      case 1:
         disableAI();
         break;
   }
}

void processDiffMenuEvents( int option ){

   switch( option ){

      case 0:
         options.ai_difficulty = 0.5;
         break;
      case 1:
         options.ai_difficulty = 0.4;
         break;
      case 2:
         options.ai_difficulty = 0.3;
         break;
      case 3: 
         options.ai_difficulty = 0.2;
         break;
   }
}
