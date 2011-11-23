#include "Parameters.h"

Parameters::Parameters( ){
   
   timed_pause = 0;

   p1_wins = 0;
   p2_wins = 0;
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
   ai_difficulty = 0.3;

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
   physics.dynamicsWorld->removeConstraint(physics.pdl1XZplaneConstraint);
   physics.dynamicsWorld->removeRigidBody(physics.paddle1RigidBody);
   delete physics.paddle1RigidBody;
   delete physics.pdl1XZplaneConstraint;

   physics.dynamicsWorld->removeConstraint(physics.pdl2XZplaneConstraint);
   physics.dynamicsWorld->removeRigidBody(physics.paddle2RigidBody);
   delete physics.paddle2RigidBody;
   delete physics.pdl2XZplaneConstraint;

   // change model and collision info
   if ( shape == ROUND ) {
      paddle1 = round1;
      paddle2 = round2;

      physics.paddle1Shape = physics.paddle1Circle;
      physics.paddle2Shape = physics.paddle2Circle;

   } else if ( shape == TRIANGLE ) {
      paddle1 = tri1;
      paddle2 = tri2;

      physics.paddle1Shape = physics.paddle1Triangle;
      physics.paddle2Shape = physics.paddle2Triangle;

   } else if ( shape == SQUARE ) {
      paddle1 = sqr1;
      paddle2 = sqr2;
      
      physics.paddle1Shape = physics.paddle1Square;
      physics.paddle2Shape = physics.paddle2Square;
   }
   
   //////////// PADDLE 1 ////////////////////////////////////////////////////////////
   // create new rigid body
      
   btVector3 paddle1Inertia(0,0,0);
   physics.paddle1Shape->calculateLocalInertia( physics.pdl_mass, paddle1Inertia );

   btRigidBody::btRigidBodyConstructionInfo paddle1RigidBodyCI(
         physics.pdl_mass,
         physics.paddle1MotionState,
         physics.paddle1Shape,
         paddle1Inertia );

   physics.paddle1RigidBody = new btRigidBody( paddle1RigidBodyCI );
   paddle1RigidBodyCI.m_friction = physics.pdl_friction;
   paddle1RigidBodyCI.m_restitution = physics.pdl_restitution;

   physics.dynamicsWorld->addRigidBody( physics.paddle1RigidBody );

   physics.paddle1RigidBody->setActivationState(DISABLE_DEACTIVATION);
   physics.paddle1RigidBody->setLinearFactor(btVector3(1,0,1));

   btTransform frameInB1 = btTransform::getIdentity();
   frameInB1.setOrigin(btVector3(0.0,0.0,0.0));

   physics.pdl1XZplaneConstraint = new btGeneric6DofConstraint(
         *physics.paddle1RigidBody,
         frameInB1,
         true);
      
   // lock the Y axis movement
   physics.pdl1XZplaneConstraint->setLinearLowerLimit( btVector3(1,0,1));
   physics.pdl1XZplaneConstraint->setLinearUpperLimit( btVector3(0,0,0));

   // lock the X, Z, rotations
   physics.pdl1XZplaneConstraint->setAngularLowerLimit(btVector3(0,1,0));
   physics.pdl1XZplaneConstraint->setAngularUpperLimit(btVector3(0,0,0));

   physics.dynamicsWorld->addConstraint(physics.pdl1XZplaneConstraint);
   
   //////////// PADDLE 2 ////////////////////////////////////////////////////////////
   // create new rigid body
      
   btVector3 paddle2Inertia(0,0,0);
   physics.paddle2Shape->calculateLocalInertia( physics.pdl_mass, paddle2Inertia );

   btRigidBody::btRigidBodyConstructionInfo paddle2RigidBodyCI(
         physics.pdl_mass,
         physics.paddle2MotionState,
         physics.paddle2Shape,
         paddle2Inertia );

   physics.paddle2RigidBody = new btRigidBody( paddle2RigidBodyCI );
   paddle2RigidBodyCI.m_friction = physics.pdl_friction;
   paddle2RigidBodyCI.m_restitution = physics.pdl_restitution;

   physics.dynamicsWorld->addRigidBody( physics.paddle2RigidBody );

   physics.paddle2RigidBody->setActivationState(DISABLE_DEACTIVATION);
   physics.paddle2RigidBody->setLinearFactor(btVector3(1,0,1));

   btTransform frameInB2 = btTransform::getIdentity();
   frameInB2.setOrigin(btVector3(0.0,0.0,0.0));

   physics.pdl2XZplaneConstraint = new btGeneric6DofConstraint(
         *physics.paddle2RigidBody,
         frameInB2,
         true);
      
   // lock the Y axis movement
   physics.pdl2XZplaneConstraint->setLinearLowerLimit( btVector3(1,0,1));
   physics.pdl2XZplaneConstraint->setLinearUpperLimit( btVector3(0,0,0));

   // lock the X, Z, rotations
   physics.pdl2XZplaneConstraint->setAngularLowerLimit(btVector3(0,1,0));
   physics.pdl2XZplaneConstraint->setAngularUpperLimit(btVector3(0,0,0));

   physics.dynamicsWorld->addConstraint(physics.pdl2XZplaneConstraint);
   
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

