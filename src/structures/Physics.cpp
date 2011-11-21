#include "Physics.h"

Physics_Model::Physics_Model(){
   
   shift_x_pos1 = 0;
   shift_x_neg1 = 0;
   shift_x_pos2 = 0;
   shift_x_neg2 = 0;
   shift_z_pos1 = 0;
   shift_z_neg1 = 0;
   shift_z_pos2 = 0;
   shift_z_neg2 = 0;

   action = 0;
}

void Physics_Model::init( vec3 const& boardSize, vec2 const& puckSize, vec2 const& paddle1Size, vec2 const& paddle2Size, 
                          vec3 const& boardCent, vec3 const& puckCent, vec3 const& paddle1Cent, vec3 const& paddle2Cent, 
                          vec4 const* board_points,  const size_t num_boardPoints, 
                          vec4 const* puck_points,   const size_t num_puckPoints, 
                          vec4 const* paddle_points, const size_t num_paddlePoints ){

   /******************************************/
   /*       General Physics Environment      */
   /******************************************/
   //create options (leave as default)
   collisionConfiguration = new btDefaultCollisionConfiguration();

   //create collision checker
   dispatcher = new	btCollisionDispatcher(collisionConfiguration);

   //create Broadphase collision checker
   overlappingPairCache = new btDbvtBroadphase();

   ///the default constraint solver. (leave as default)
   solver = new btSequentialImpulseConstraintSolver;

   ///create dynamic environment 
   dynamicsWorld = new btDiscreteDynamicsWorld( dispatcher, overlappingPairCache, solver, collisionConfiguration);

   ///set value of gravity
   dynamicsWorld->setGravity(btVector3(0,-10,0));


   /******************************************/
   /*                   Board                */
   /******************************************/
   btTriangleMesh *mTriMesh = new btTriangleMesh();

   vec3 a0, a1, a2;
   for(size_t i=0; i<num_boardPoints; i+=3){

      a0 = vec3( board_points[i+0].x, board_points[i+0].y, board_points[i+0].z );
      a1 = vec3( board_points[i+1].x, board_points[i+1].y, board_points[i+1].z );
      a2 = vec3( board_points[i+2].x, board_points[i+2].y, board_points[i+2].z );

      // For whatever your source of triangles is
      //   give the three points of each triangle:
      btVector3 v0(a0.x,a0.y,a0.z);
      btVector3 v1(a1.x,a1.y,a1.z);
      btVector3 v2(a2.x,a2.y,a2.z);

      // Then add the triangle to the mesh:
      mTriMesh->addTriangle(v0,v1,v2);
   }

   btCollisionShape *boardShape = new btBvhTriangleMeshShape(mTriMesh,true);

   // Now use mTriMeshShape as your collision shape.
   // Everything else is like a normal rigid body

   //build motion state (BOARD)
   boardMotionState = new btDefaultMotionState( btTransform(btQuaternion(0,0,0,1),btVector3( 0, -0.1,0)));

   btRigidBody::btRigidBodyConstructionInfo boardRigidBodyCI( 0, boardMotionState, boardShape, btVector3(0,0,0));
   boardRigidBodyCI.m_friction = 0.01;     //this is the friction of its surfaces
   
   boardRigidBody = new btRigidBody( boardRigidBodyCI);

   dynamicsWorld->addRigidBody( boardRigidBody );

   /******************************************/
   /*                    Puck                */
   /******************************************/
   ///build puck collision model
   puckShape = new btCylinderShape(btVector3(btScalar(puckSize.x),btScalar(0.1),btScalar(puckSize.x)));

   //build motion state (PUCK)
   puckMotionState = new btDefaultMotionState( btTransform(btQuaternion(0,0,0,1), btVector3( 0,0.0,0)));

   btScalar puck_mass = 5;
   btVector3 puckInertia(0,0,0);
   puckShape->calculateLocalInertia( puck_mass, puckInertia);

   btRigidBody::btRigidBodyConstructionInfo puckRigidBodyCI( puck_mass, puckMotionState, puckShape, puckInertia);
   puckRigidBodyCI.m_friction = 0.00;     //this is the friction of its surfaces
   puckRigidBodyCI.m_restitution = 0.8;   //this is the "bounciness"

   puckRigidBody = new btRigidBody( puckRigidBodyCI );
   dynamicsWorld->addRigidBody( puckRigidBody );

   puckRigidBody->setActivationState(DISABLE_DEACTIVATION);
   puckRigidBody->setLinearFactor(btVector3(1,0,1));

   /******************************************/
   /*                  Paddle1               */
   /******************************************/
   ///build paddle1 collision model
   paddle1Shape = new btCylinderShape(btVector3(btScalar(paddle1Size.x),btScalar(0.1),btScalar(paddle1Size.x)));

   //build motion state (PADDLE1)
   paddle1MotionState = new btDefaultMotionState( btTransform(btQuaternion(0,0,0,1), btVector3( paddle1Cent.x,0.0, paddle1Cent.z)));

   btScalar paddle1_mass = 15;
   btVector3 paddle1Inertia(0,0,0);
   paddle1Shape->calculateLocalInertia( paddle1_mass, paddle1Inertia);

   btRigidBody::btRigidBodyConstructionInfo paddle1RigidBodyCI( paddle1_mass, paddle1MotionState, paddle1Shape, paddle1Inertia);
   paddle1RigidBody = new btRigidBody( paddle1RigidBodyCI );
   paddle1RigidBodyCI.m_friction = 0.00;     //this is the friction of its surfaces
   paddle1RigidBodyCI.m_restitution = 0.8;   //this is the "bounciness"
   
   dynamicsWorld->addRigidBody( paddle1RigidBody );

   paddle1RigidBody->setActivationState(DISABLE_DEACTIVATION);
   paddle1RigidBody->setLinearFactor(btVector3(1,0,1));

   /******************************************/
   /*                  Paddle2               */
   /******************************************/
   ///build paddle2 collision model
   paddle2Shape = new btCylinderShape(btVector3(btScalar(paddle2Size.x),btScalar(0.1),btScalar(paddle2Size.x)));

   //build motion state (PADDLE2)
   paddle2MotionState = new btDefaultMotionState( btTransform(btQuaternion(0,0,0,1), btVector3( paddle2Cent.x,0.0, paddle2Cent.z)));

   btScalar paddle2_mass = 15;
   btVector3 paddle2Inertia(0,0,0);
   paddle2Shape->calculateLocalInertia( paddle2_mass, paddle2Inertia);

   btRigidBody::btRigidBodyConstructionInfo paddle2RigidBodyCI( paddle2_mass, paddle2MotionState, paddle2Shape, paddle2Inertia);
   paddle2RigidBody = new btRigidBody( paddle2RigidBodyCI );
   paddle2RigidBodyCI.m_friction = 0.00;     //this is the friction of its surfaces
   paddle2RigidBodyCI.m_restitution = 0.8;   //this is the "bounciness"
   
   dynamicsWorld->addRigidBody( paddle2RigidBody );

   paddle2RigidBody->setActivationState(DISABLE_DEACTIVATION);
   paddle2RigidBody->setLinearFactor(btVector3(1,0,1));

}

