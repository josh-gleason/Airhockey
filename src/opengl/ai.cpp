#include "ai.h"

void aiTimer(int value)
{
   // get puck position
   vec2 puck_pos = vec2( options.puck->get_translation().x, options.puck->get_translation().z );
   vec2 paddle_pos = vec2( options.paddle1->get_translation().x, options.paddle1->get_translation().z );
   vec2 goal_pos = vec2( -3.0, 0.0 );  // approximate

   vec2 paddle_puck = puck_pos - paddle_pos;
   GLfloat paddle_puck_dist = max(0.0001f, sqrt(paddle_puck.x*paddle_puck.x+paddle_puck.y*paddle_puck.y));
   paddle_puck /= paddle_puck_dist; // normalize;

   vec2 paddle_goal = goal_pos - paddle_pos;
   GLfloat paddle_goal_dist = max(0.0001f, sqrt(paddle_goal.x*paddle_goal.x+paddle_goal.y*paddle_goal.y));
   paddle_goal /= paddle_goal_dist; // normalize;
   
   vec2 puck_goal = goal_pos - puck_pos;
   GLfloat puck_goal_dist = max(0.0001f,sqrt(puck_goal.x*puck_goal.x+puck_goal.y*puck_goal.y));
   puck_goal /= puck_goal_dist;  // normalize

   vec2 puck_velocity = vec2(options.puck->get_velocity().x,options.puck->get_velocity().z);
   GLfloat puck_speed = max(0.0001f,sqrt(puck_velocity.x*puck_velocity.x+puck_velocity.y*puck_velocity.y));

   GLfloat puck_goal_ratio = paddle_puck_dist / paddle_goal_dist;

   vec2 movement = paddle_puck*puck_goal_ratio +
                   paddle_goal*puck_speed/puck_goal_ratio;

   GLfloat move_dist = max(0.0001f,sqrt(movement.x*movement.x+movement.y*movement.y));

   // normalize movement
   if ( move_dist > options.pdl1_maxVelocity )
      movement = movement / move_dist * options.pdl1_maxVelocity;

   // if switching direction cut in half again
   vec2 paddle_velocity = vec2(options.paddle1->get_velocity().x,options.paddle1->get_velocity().z);
   vec2 sum = movement + paddle_velocity;

   // special case to help unstick puck down near goal
   static int stuck_frames;
   if ( puck_pos.x < -3.31 && fabs(puck_velocity.x) < 0.05 )
   {
      stuck_frames++;
      if ( stuck_frames > 10 )
      {
         // apply force to the puck
         options.physics.puckRigidBody->applyCentralForce(btVector3(100,0.0,0));
         movement.x = options.pdl1_maxVelocity;
         movement.y = 0.0;
      }
   }
   else
      stuck_frames = 0;

   // move paddle
   options.set_paddle1_dest( paddle_pos + movement );

   if ( options.ai_enabled )
      glutTimerFunc(17, aiTimer, 0);
}

void enableAI()
{
   if ( options.ai_enabled ) return;

   options.ai_enabled = true;
   
   glutTimerFunc(17, aiTimer, 0);
}

void disableAI()
{
   options.ai_enabled = false;
}
