/**
  * @file timer.cpp
  * Implementation of timer handler
  * @author Marvin Smith
*/
#include "timer.h"

void timerHandle( int state ){

/*
   // set paddle back to dynamic state
   options.physics.dynamicsWorld->removeRigidBody(options.physics.paddle1RigidBody);
   btVector3 inertia(0,0,0);
   options.physics.paddle1Shape->calculateLocalInertia(options.physics.pdl_mass, inertia);
   options.physics.paddle1RigidBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
   options.physics.paddle1RigidBody->setMassProps(options.physics.pdl_mass, inertia);
   options.physics.paddle1RigidBody->updateInertiaTensor();
   options.physics.paddle1RigidBody->setLinearVelocity(btVector3(0,0,0));
   options.physics.paddle1RigidBody->setAngularVelocity(btVector3(0,0,0));
   options.physics.paddle1RigidBody->setActivationState(WANTS_DEACTIVATION);
   options.physics.dynamicsWorld->addRigidBody(options.physics.paddle1RigidBody);
*/
   vec3 motion;

   //step simulation
   options.physics.dynamicsWorld->stepSimulation(1.f/60.f);
   
   //keep constant upward force on puck and paddles
   //btVector3 force = btVector3(0,0.5,0);
   //options.physics.puckRigidBody->applyCentralForce(force); 
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

//   vec3 loc_orig = options.paddle1->get_translation();

   // make paddle not move
   options.physics.paddle1RigidBody->setLinearVelocity(btVector3(0.0,0.0,0.0));
   options.physics.paddle1RigidBody->setAngularVelocity(btVector3(0.0,0.0,0.0));

   //compute required force to move paddle to paddle1_dest in next step
   btVector3 pos_f, pos_i, dist;
   btScalar t = 1.f/60.f,
         mass = options.physics.pdl_mass;

   pos_f = btVector3(options.paddle1_dest[0], 0.0, options.paddle1_dest[1]);
   pos_i = btVector3(motion.x, 0.0, motion.z);

   //dist = (pos_f - pos_i);
   //vel_i = (pos_i - btVector3(loc_orig.x, 0.0, loc_orig.z))*60.f;

   btScalar f_xVal = (pos_f.getX()-pos_i.getX())*mass/t/t;
   btScalar f_zVal = (pos_f.getZ()-pos_i.getZ())*mass/t/t;

   //btScalar len = sqrt(f_xVal*f_xVal+f_zVal*f_zVal);

   std::cout << "APPLYING FORCE" << std::endl;
   btVector3 F = btVector3( f_xVal, 0.0, f_zVal );
   options.physics.paddle1RigidBody->applyCentralForce(F);
   
   //options.paddle1RigidBody->setLinearVelocity(btVector3(0.0,0.0,0.0));
   //options.paddle1RigidBody->setAngularVelocity(btVector3(0.0,0.0,0.0));

   std::cout << "Position I   : " << pos_i.getX() << ", " << pos_i.getZ() << std::endl;
   std::cout << "Position F   : " << pos_f.getX() << ", " << pos_f.getZ() << std::endl;
   std::cout << "Desired Force: " << f_xVal << ", " << f_zVal << std::endl;
   std::cout << std::endl;

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

   // Convert paddle to kinematic object before translating
/*
   options.physics.dynamicsWorld->removeRigidBody(options.physics.paddle1RigidBody);
   options.physics.paddle1RigidBody->setMassProps(0, btVector3(0,0,0));
   options.physics.paddle1RigidBody->setCollisionFlags(options.physics.paddle1RigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
   options.physics.paddle1RigidBody->setLinearVelocity(btVector3(0,0,0));
   options.physics.paddle1RigidBody->setAngularVelocity(btVector3(0,0,0));
   options.physics.paddle1RigidBody->setActivationState(DISABLE_DEACTIVATION);
   options.physics.dynamicsWorld->addRigidBody(options.physics.paddle1RigidBody);
*/
}
