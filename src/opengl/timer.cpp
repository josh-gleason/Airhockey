/**
  * @file timer.cpp
  * Implementation of timer handler
  * @author Marvin Smith
*/
#include "timer.h"

void timerHandle( int state ){
   
   vec3 motion;

   //step simulation
   options.physics.dynamicsWorld->stepSimulation(1.f/60.f,10);
   
   //keep constant upward force on puck and paddles
   btVector3 force = btVector3(0,0.5,0);
   options.physics.puckRigidBody->applyCentralForce(force); 
   //options.physics.paddle1RigidBody->applyCentralForce(force); 
   //options.physics.paddle2RigidBody->applyCentralForce(force); 
  
   /*********************/
   /*       Puck        */
   /*********************/
   //get an update of the motion state
   options.physics.puckRigidBody->getMotionState()->getWorldTransform( options.physics.puck_trans );
   
   //load new position into structure for puck model
   motion = vec3( options.physics.puck_trans.getOrigin().getX(), options.physics.puck_trans.getOrigin().getY(), options.physics.puck_trans.getOrigin().getZ());

   //push new position into puck
   options.puck->set_translation( motion );

   /*********************/
   /*      Paddle1      */
   /*********************/
   //get an update of the motion state
   options.physics.paddle1RigidBody->getMotionState()->getWorldTransform( options.physics.paddle1_trans );
   
   //load new position into structure for paddle1 model
   motion = vec3( options.physics.paddle1_trans.getOrigin().getX(), options.physics.paddle1_trans.getOrigin().getY(), options.physics.paddle1_trans.getOrigin().getZ());

   //push new position into paddle1
   options.paddle1->set_translation( motion );

   /*********************/
   /*      Paddle2      */
   /*********************/
   //get an update of the motion state
   options.physics.paddle2RigidBody->getMotionState()->getWorldTransform( options.physics.paddle2_trans );

   //load new position into structure for paddle2 model
   motion = vec3( options.physics.paddle2_trans.getOrigin().getX(), options.physics.paddle2_trans.getOrigin().getY(), options.physics.paddle2_trans.getOrigin().getZ());

   //push new position into paddle2
   options.paddle2->set_translation( motion );



   /**************************************************/
   /*         Check if the puck has scored           */
   /**************************************************/
   vec3 position = vec3( options.physics.puck_trans.getOrigin().getX(), options.physics.puck_trans.getOrigin().getY(), options.physics.puck_trans.getOrigin().getZ());
   if (position.x > 3.05){
      if(position.z > -0.2 && position.z < 0.2 ){
         options.p2_score++;
      }
   }
   if (position.x < -3.05){
      if(position.z > -0.2 && position.z < 0.2 ){
         options.p1_score++;
      }
   }


   glutPostRedisplay(); 
   glutTimerFunc( 20, &timerHandle, 0);

}
