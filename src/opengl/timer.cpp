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

   /////////// MOVE PADDLE /////////////////////////////////////////////////////
   {
      // reduce velocity to zero
      options.physics.paddle1RigidBody->setLinearVelocity(btVector3(0.0,0.0,0.0));
      options.physics.paddle1RigidBody->setAngularVelocity(btVector3(0.0,0.0,0.0));

      //compute required force to move paddle to paddle1_dest in next step
      btVector3 pos_f, pos_i, dist;
      btScalar t = 1.f/60.f,
            mass = options.physics.pdl_mass;

      pos_f = btVector3(options.paddle1_dest[0], 0.0, options.paddle1_dest[1]);
      pos_i = btVector3(motion.x, 0.0, motion.z);

      btScalar f_xVal = (pos_f.getX()-pos_i.getX())*mass/t/t;
      btScalar f_zVal = (pos_f.getZ()-pos_i.getZ())*mass/t/t;

      if ( options.pdl_maxVelocity > 0 )
      {
         // compute what the velocity will be
         btVector3 vel_new = btVector3(f_xVal * t / mass, 0.0, f_zVal * t / mass);
         btScalar vel_newMag = sqrt(vel_new.getX()*vel_new.getX()+vel_new.getZ()*vel_new.getZ());
         
         // if the new velocity is greater, scale the force by the ratio
         // Note: can be done b/c velocity is proportional to Force iff initial velocity=0
         //       and I know its zero because I just set it to zero :D
         if ( vel_newMag > options.pdl_maxVelocity )
         {
            // scale down the force to limit the velocity
            f_xVal = f_xVal / vel_newMag * options.pdl_maxVelocity;
            f_zVal = f_zVal / vel_newMag * options.pdl_maxVelocity;
         }
      }

      // apply a force to the paddle
      btVector3 F = btVector3( f_xVal, 0.0, f_zVal );
      options.physics.paddle1RigidBody->applyCentralForce(F);
   }
   /////////////////////////////////////////////////////////////////////////////

   /*********************/
   /*      Paddle2      */
   /*********************/
   //get an update of the motion state
   options.physics.paddle2RigidBody->getMotionState()->getWorldTransform( options.physics.paddle2_trans );

   //load new position into structure for paddle2 model
   motion = vec3( options.physics.paddle2_trans.getOrigin().getX(), options.physics.paddle2_trans.getOrigin().getY(), options.physics.paddle2_trans.getOrigin().getZ());

   //push new position into paddle2
   options.paddle2->set_translation( motion );

   /////////// MOVE PADDLE /////////////////////////////////////////////////////
   {
      // reduce velocity to zero
      options.physics.paddle2RigidBody->setLinearVelocity(btVector3(0.0,0.0,0.0));
      options.physics.paddle2RigidBody->setAngularVelocity(btVector3(0.0,0.0,0.0));

      //compute required force to move paddle to paddle2_dest in next step
      btVector3 pos_f, pos_i, dist;
      btScalar t = 1.f/60.f,
            mass = options.physics.pdl_mass;

      pos_f = btVector3(options.paddle2_dest[0], 0.0, options.paddle2_dest[1]);
      pos_i = btVector3(motion.x, 0.0, motion.z);

      btScalar f_xVal = (pos_f.getX()-pos_i.getX())*mass/t/t;
      btScalar f_zVal = (pos_f.getZ()-pos_i.getZ())*mass/t/t;

      if ( options.pdl_maxVelocity > 0 )
      {
         // compute what the velocity will be
         btVector3 vel_new = btVector3(f_xVal * t / mass, 0.0, f_zVal * t / mass);
         btScalar vel_newMag = sqrt(vel_new.getX()*vel_new.getX()+vel_new.getZ()*vel_new.getZ());
         
         // if the new velocity is greater, scale the force by the ratio
         // Note: can be done b/c velocity is proportional to Force iff initial velocity=0
         //       and I know its zero because I just set it to zero :D
         if ( vel_newMag > options.pdl_maxVelocity )
         {
            // scale down the force to limit the velocity
            f_xVal = f_xVal / vel_newMag * options.pdl_maxVelocity;
            f_zVal = f_zVal / vel_newMag * options.pdl_maxVelocity;
         }
      }

      // apply a force to the paddle
      btVector3 F = btVector3( f_xVal, 0.0, f_zVal );
      options.physics.paddle2RigidBody->applyCentralForce(F);
   }
   /////////////////////////////////////////////////////////////////////////////

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
   glutTimerFunc( 17, &timerHandle, 0);
}
