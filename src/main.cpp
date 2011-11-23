//
// Display a color cube
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an orthographic projection
//   as the default projetion.
#include <algorithm>
#include <iomanip>

// math/
#include "mat.h"
#include "vector.h"

// opengl/
#include "shader_processing.h"
#include "display.h"
#include "keyboard.h"
#include "mouse.h"
#include "timer.h"
#include "reshape.h"
#include "parser.h"
#include "ai.h"

// structures/
#include "Parameters.h"

// model_loader/
#include "objLoader.h"
#include "print_data.h"

// bullet physics engine
#include <btBulletDynamicsCommon.h>

int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

Parameters options;

//----------------------------------------------------------------------------
// OpenGL initialization
void init()
{
   // enable anti-aliasing (and transparency)
   glEnable(GL_LINE_SMOOTH);
   glEnable(GL_POLYGON_SMOOTH);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   //// Camera Setup
   // set perspective
   options.zNear = 0.25;
   options.zFar  = 50.0;
   options.fova  = 30.0;
   options.ratio = 1.0; 

   options.projectionMatrix = Perspective(options.fova, options.ratio,
         options.zNear, options.zFar);

   // set up initial camera position
   options.cam_timerStep = 10;
   options.cam_lookStep = 1.0;
   options.cam_moveStep = 0.06;

   options.camera = Camera(vec4(8.0,5.0,0.0,1.0), options.cam_mode);
   options.camera.rotateHoriz(90);
   options.camera.rotateVert(-35);

   // set up initial light position
   options.light_timerStep = 10;
   options.light_moveStep = 0.06;

   // Load shaders and use the resulting shader program
   options.program = InitShader( "data/shaders/vshader.glsl", "data/shaders/fshader.glsl" );
   glUseProgram( options.program );

   glEnable( GL_DEPTH_TEST );
   glClearColor( 0.5, 0.5, 0.5, 1.0 );

   //initialize Air Hockey Board

   options.board = new Object("data/models/Airhockey.obj", 0.5, options.program);
   Object walls("data/models/walls.obj", 0.5, options.program);   // for physics
   options.puck = new Object("data/models/Puck.obj", 0.2, options.program);
   options.paddle1 = new Object("data/models/Paddle.obj", 0.2, options.program);
   options.paddle2 = new Object("data/models/Paddle.obj", 0.2, options.program);

   options.board->init_buffers( options.light.m_position, options.light.m_ambient, options.light.m_diffuse, options.light.m_specular);
   walls.init_buffers( options.light.m_position, options.light.m_ambient, options.light.m_diffuse, options.light.m_specular, false );  // dont load shader
   options.puck->init_buffers( options.light.m_position, options.light.m_ambient, options.light.m_diffuse, options.light.m_specular);
   options.paddle1->init_buffers( options.light.m_position, options.light.m_ambient, options.light.m_diffuse, options.light.m_specular);
   options.paddle2->init_buffers( options.light.m_position, options.light.m_ambient, options.light.m_diffuse, options.light.m_specular);

   //set initial object positions
   options.paddle1->adjust_translation( vec3(-3.0, 0.0, 0.0));
   options.paddle2->adjust_translation( vec3( 3.0, 0.0, 0.0));
   options.set_paddle1_dest(vec2(-3.0, 0.0)); // (x,z) coords
   options.set_paddle2_dest(vec2(3.0, 0.0));  // (x,z) coords
	
   /*********************************************/
   /*             Physics Engine                */
   /*********************************************/
   vec3 boardSize = options.board->get_dimensions();

   double radius = options.puck->get_x_width()/2.0;
   double height = options.puck->get_y_width();
   vec2 puckSize  = vec2( radius, height);
   
   radius = options.paddle1->get_x_width()/2.0;
   height = options.paddle1->get_y_width();
   vec2 paddle1Size = vec2( radius, height );
   //cout << "Height_base: " << height/2.0 << endl;

   radius = options.paddle2->get_x_width()/2.0;
   height = options.paddle2->get_y_width();
   vec2 paddle2Size = vec2( radius, height );
    
   vec3 boardCent   = options.board->get_centroid();
   vec3 puckCent    = options.puck->get_centroid();
   vec3 paddle1Cent = options.paddle1->get_centroid();
   vec3 paddle2Cent = options.paddle2->get_centroid();

   options.physics.init( boardSize, puckSize, paddle1Size, paddle2Size, 
                         boardCent, puckCent, paddle1Cent, paddle2Cent,
                         walls.get_vertices(),  walls.num_verts(),
                         options.puck->get_vertices(),   options.puck->num_verts(),
                         options.paddle1->get_vertices(), options.paddle1->num_verts());


   enableAI();
}

//----------------------------------------------------------------------------
int main( int argc, char **argv )
{
   parse_arguements( argc, argv, options );

   cout << "================ Camera Controls ================" << endl
        << "============ (w,a,s,d,q,e) movement =============" << endl
        << " w - Move Forward          s - Move Backwards    " << endl
        << " a - Move Left             d - Move Right        " << endl
        << " q - Move Up               e - Move Down         " << endl
        << "============== (u,h,j,k,y,i) look ===============" << endl
        << " i - Look Up               k - Look Down         " << endl
        << " j - Look Left             l - Look Right        " << endl
        << "== Twist works only if camera is in free mode ===" << endl
        << " u - Twist CCW             o - Twist CW          " << endl
        << "=================== lighting model ==============" << endl 
        << " n - move light -z axis   m - move light +z axis " << endl
        << " , - move light -x axis   . - move light +x axis " << endl
        << "=================== change paddle  ==============" << endl 
        << " x - change paddle                               " << endl
        << "=================================================" << endl << endl;

   srand(time(0));

   glutInit( &argc, argv );
   glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
   glutInitWindowSize( options.glut_window_x, options.glut_window_y );
   glutInitWindowPosition(options.glut_initial_x,options.glut_initial_y);
   glutCreateWindow( options.glut_window_name.c_str() );

#ifndef __APPLE__
   glewInit();
#endif
   init();

   glutSetKeyRepeat( GLUT_KEY_REPEAT_OFF );

   glutDisplayFunc( display_function );

   // Keyboard down/up functions
   glutKeyboardFunc( keyboardPress );
   glutKeyboardUpFunc( keyboardUp );
   glutSpecialFunc( special_keys);
   glutSpecialUpFunc( special_Upkeys);

   // Mouse func
   glutMouseFunc( mouseEvent );

   // Dragging the mouse
   glutMotionFunc( mouseMove );

   // When the screen is resized
   glutReshapeFunc( reshape );

   // timer function
   glutTimerFunc( 17, timerHandle, 0);

   glutMainLoop();
   return 0;
}
