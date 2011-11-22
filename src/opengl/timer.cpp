/**
 * @file timer.cpp
 * Implementation of timer handler
 * @author Marvin Smith
 */
#include "timer.h"

void timerHandle( int state ){
      
   //step simulation
   options.physics.dynamicsWorld->stepSimulation(1.f/60.f,10);
   vec3 motion;

   // what are these for? -Josh
   static bool p1flag = false;
   static bool p2flag = false;

   /**************************************************/
   /*         Check if the puck has scored           */
   /**************************************************/
   /*    Check to make sure that the puck has not passed  */
   /*    through the board and does not need to be reset  */
   vec3 puck_pos = vec3( options.physics.puck_trans.getOrigin().getX(), options.physics.puck_trans.getOrigin().getY(), options.physics.puck_trans.getOrigin().getZ());

   static bool p1scored = false, p2scored = false, ob = false;

   // first check if scoreing is possible
   
   if ( puck_pos.z > 1.949742 || puck_pos.z < -1.949742 ) { // ob
      ob = true;
   }
   else if ( puck_pos.z < 0.374856f && puck_pos.z > -0.374856f ) {  // scoring possible
      if ( puck_pos.x > 3.625 )
         p1scored = true;
      else if ( puck_pos.x < -3.625 )
         p2scored = true;
   }
   else if ( puck_pos.x > 3.625 || puck_pos.x < -3.625 ) {  // ob
      ob = true;
   }

   if ( p1scored || p2scored || ob )
   {
      vec3 position = vec3( options.physics.puck_trans.getOrigin().getX(), options.physics.puck_trans.getOrigin().getY(), options.physics.puck_trans.getOrigin().getZ());
      
      //set puck position (and add scores)
      if ( p2scored ) //&& !p2flag )
      {
         options.physics.puckRigidBody->setWorldTransform(btTransform(btQuaternion(1,0,0,0),btVector3( 1.2, 0, 0)));
         options.p2_score++;
         p2flag = true;
      }
      //else if( p2scored && p2flag){
      //   p2flag = false;
      //}
      else if ( p1scored ) //&& !p1flag )
      {
         options.physics.puckRigidBody->setWorldTransform(btTransform(btQuaternion(1,0,0,0),btVector3(-1.2, 0, 0)));
         options.p1_score++;
         p1flag = true;
      }
      //else if( p1scored && p1flag ){
      //   p1flag = false;
      //}
      else if ( ob )
      {
         options.physics.puckRigidBody->setWorldTransform(btTransform(btQuaternion(1,0,0,0),btVector3(0, 0, 0)));
      }
      
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



   }
   else{ // puck in play

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

   }
   glutPostRedisplay(); 
   glutTimerFunc( 17, &timerHandle, 0);
}
