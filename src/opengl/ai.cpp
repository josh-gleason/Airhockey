#include "ai.h"

void aiTimer(int value)
{
   // get puck position
   vec2 puck_pos = vec2( options.physics.puck_trans.getOrigin().getX(), options.physics.puck_trans.getOrigin().getZ());

   options.set_paddle1_dest(puck_pos);

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
