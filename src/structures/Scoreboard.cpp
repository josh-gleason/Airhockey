#include "Scoreboard.h"

/*****************************************/
/*              Load an image            */
/*****************************************/
unsigned char* Scoreboard::read_image(char* fname)
{
   int Q;
   unsigned char *charImage;
   char header [100], *ptr; 
   ifstream ifp;

   ifp.open(fname, ios::in | ios::binary);

   if (!ifp) {
      cout << "Can't read image: " << fname << endl;
      exit(1);
   }

   // read header

   ifp.getline(header,100,'\n');

   if ( (header[0]!=80) ||    /* 'P' */
         (header[1]!=54) ) {   /* '6' */
      cout << header[0] << ", " << header[1] << endl;
      cout << "Image " << fname << " is not PPM" << endl;
      exit(1);
   }

   ifp.getline(header,100,'\n');
   while(header[0]=='#')
      ifp.getline(header,100,'\n');

   img_rows=strtol(header,&ptr,0);
   img_cols=atoi(ptr);

   ifp.getline(header,100,'\n');
   Q=strtol(header,&ptr,0);

   charImage = new unsigned char [3*img_rows*img_cols];
   unsigned char* tImage    = new unsigned char [3*img_rows*img_cols];

   ifp.read( reinterpret_cast<char *>(charImage), (3*img_cols*img_rows)*sizeof(unsigned char));

   if (ifp.fail()) {
      cout << "Image " << fname << " has wrong size" << endl;
      exit(1);
   }

   ifp.close();

   for(int i=0; i<(3*img_rows*img_cols); i++)
      tImage[i] = charImage[(3*img_rows*img_cols-1)-i];
   for(int i=0; i<(3*img_rows*img_cols); i++)
      charImage[i] = tImage[i];
   
   delete tImage;
   return charImage;
}
// Draw the normals for the object
//#define DEBUG_DRAW_NORMALS

/*****************************************/
/*             Constructors              */
/*****************************************/
Scoreboard::Scoreboard( ){


}

Scoreboard::~Scoreboard( ){

   make_empty();

}

void Scoreboard::make_empty(){
   if( points_main != NULL )
      delete [] points_main;
   if( tex_coords_main != NULL )
      delete [] tex_coords_main;
}


void Scoreboard::init( const GLuint& prog ){
   
   
   //read image from file
   img_data = read_image( "data/characters_flip.ppm" );


   /*********************************/
   /*       High Score Banner       */
   /*********************************/
   //build points and tex_coords arrays
   points_main = NULL;
   tex_coords_main = NULL;
   vec4 tl_tex(-0.75, 0.3, 0, 1);
   vec4 br_tex( 0.75,-0.3, 0, 1);
   build_texture_values( tl_tex, br_tex, string("HIGH SCORES"), points_main, tex_coords_main, vex_size_main, tex_size_main);
   
   num_points_main = tex_size_main/sizeof(vec2);

   // Initialize texture objects
   glGenTextures( 1, &texture );
   
   //bind and build data
   glBindTexture( GL_TEXTURE_2D, texture );
   
   //load texture image data
   glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, img_rows, img_cols, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data );
   
   //set texture wrap mode x axis
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
   
   //set texture wrap mode y axis
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
   
   //set magnification mode
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
   
   //set minification mode
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

   //activate single texture
   glActiveTexture( GL_TEXTURE0 );

   // Create a vertex array object
   GLuint vao;
   
   // generate and bind the vao
   #ifdef __APPLE__
   glGenVertexArraysAPPLE( 1, &vao );
   glBindVertexArrayAPPLE( vao );
   #else
   glGenVertexArrays(1, &vao);
   glBindVertexArrayAPPLE( vao );
   #endif

   // Create and initialize a buffer object
   GLuint buffer;
   glGenBuffers( 1, &buffer );
   glBindBuffer( GL_ARRAY_BUFFER, buffer );
   glBufferData( GL_ARRAY_BUFFER, vex_size_main + tex_size_main, NULL, GL_STATIC_DRAW );

   // Specify an offset to keep track of where we're placing data in our
   //   vertex array buffer.  We'll use the same technique when we
   //   associate the offsets with vertex attribute pointers.
   GLintptr offset = 0;
   glBufferSubData( GL_ARRAY_BUFFER, offset, vex_size_main, points_main );
   offset += vex_size_main;

   glBufferSubData( GL_ARRAY_BUFFER, offset, tex_size_main, tex_coords_main );

   // Load shaders and use the resulting shader program
   program = prog;
   glUseProgram( program );

   // set up vertex arrays
   offset = 0;
   GLuint vPosition = glGetAttribLocation( program, "vPosition" );
   glEnableVertexAttribArray( vPosition );
   glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset) );
   offset += vex_size_main;

   GLuint vTexCoord = glGetAttribLocation( program, "vTexCoord" );
   glEnableVertexAttribArray( vTexCoord );
   glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset) );

   glEnable( GL_DEPTH_TEST );

}



void Scoreboard::draw_shape(  ){

   glUseProgram(program);
#ifdef __APPLE__
   glBindVertexArrayAPPLE( vao );
#else
   glBindVertexArray( vao ); 
#endif
   glDrawArrays( GL_TRIANGLES, 0, num_points_main );


}

size_t Scoreboard::get_index( char const& letter ){
   if( letter >= '0' && letter <= '9' )
      return ((letter-'0')+26);
   else if( letter >= 'A' && letter <= 'Z' )
      return (letter-'A');
   else if( letter >= 'a' && letter <= 'z' )
      return (letter-'a');
   else
      return 36;

}

void Scoreboard::build_texture_values( vec4 const& tl, vec4 const& br, string const& tag, vec4*& loc, vec2*& char_data, GLuint& vex_size, GLuint& tex_size){

   //find intervals for letters
   double ranX = (br.x-tl.x)/(double)tag.size();

   //create array of vertices corresponding to locations
   size_t size_loc = 6 * tag.size(); // 6 verts per char, then the number of letters
   loc = new vec4[size_loc];

   //iterate over interval to set in place the pixel locations
   double beg_x, end_x, z_val;
   for( size_t i=0; i<tag.size(); i++){
      //compute x ranges
      beg_x = i*ranX+tl.x; end_x = beg_x + ranX;
      
      //interpolate z value for step
      z_val = (1 - (i/(double)tag.size()))*tl.z + (i/(double)tag.size())*br.z;
      
      //load values into vertex array
      loc[i*6+0] = vec4( beg_x, br.y, z_val, 1);//bl
      loc[i*6+1] = vec4( end_x, br.y, z_val, 1);//br
      loc[i*6+2] = vec4( end_x, tl.y, z_val, 1);//tr
      loc[i*6+3] = vec4( beg_x, br.y, z_val, 1);//bl
      loc[i*6+4] = vec4( end_x, tl.y, z_val, 1);//tr
      loc[i*6+5] = vec4( beg_x, tl.y, z_val, 1);//tl
   
   }
   vex_size = sizeof(vec4)*6*tag.size();

   //now that we have the vertex locations built, lets go ahead
   //and figure out which characters go into the overall texture
   
   //initialize char_data array to store texture locations
   size_t size_char = 6 * tag.size();
   char_data = new vec2[size_char];

   //iterate over interval to set in place the texture locations
   size_t idx;
   for( size_t i=0; i<tag.size(); i++){
      
      //figure out which letter we are dealing with
      idx = get_index(tag[i]);

      //load values into vertex array
      char_data[i*6+0] = vec2( char_ran[idx].x, 0.0);//bl
      char_data[i*6+1] = vec2( char_ran[idx].y, 0.0);//br
      char_data[i*6+2] = vec2( char_ran[idx].y, 1.0);//tr
      char_data[i*6+3] = vec2( char_ran[idx].x, 0.0);//bl
      char_data[i*6+4] = vec2( char_ran[idx].y, 1.0);//tr
      char_data[i*6+5] = vec2( char_ran[idx].x, 1.0);//tl

   }
   tex_size = sizeof(vec2)*6*tag.size();

}


