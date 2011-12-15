#include "Object.h"

// Draw the normals for the object
//#define DEBUG_DRAW_NORMALS

Object::Object( const string& fname, const double& sf, const GLuint prog ){

   program  = prog;
   scale_factor = sf;
   model_name = fname;
   velocity = vec4(0.0,0.0,0.0,1.0);

}

Object::~Object(){

}

void Object::init_buffers( const vec4& lpos, const vec4& l_amb, const vec4& l_diff, const vec4& l_spec, bool load_shader, int rotate){

   vector<GLfloat> shins;
   
   //build rotation matrix
   double radians = rotate * M_PI / 180.0;
   mat4 rotation(cos(radians),  0.0, sin(radians), 0.0,
               0.0,           1.0, 0.0,          0.0,
               -sin(radians), 0.0, cos(radians), 0.0,
               0.0,           0.0, 0.0,          1.0);
   
   //load vbo array data
   Load_Model_Data( model_name, rotation, scale_factor, vertices, normals, ambients, diffuses, speculars, shins, vertex_count );
   vertices_size = sizeof(vec4)*vertex_count;
   shininess_size = sizeof(GLfloat)*vertex_count;
   
   /*************************************************/
   /*    Iterate through materials, multiplying the */
   /*    light material to the material.            */
   /*************************************************/
   for(size_t i=0; i<vertex_count; i++){
      ambients[i]  *= l_amb;
      diffuses[i]  *= l_diff;
      speculars[i] *= l_spec;
   }
   
   ///Get width of object
   double x_min = vertices[0].x;
   double x_max = vertices[0].x;
   double y_min = vertices[0].y;
   double y_max = vertices[0].y;
   double z_min = vertices[0].z;
   double z_max = vertices[0].z;
   for(size_t i=1; i<vertices.size(); i++){
      if( vertices[i].x < x_min )
         x_min = vertices[i].x;
      if( vertices[i].x > x_max )
         x_max = vertices[i].x;
      if( vertices[i].y < y_min )
         y_min = vertices[i].y;
      if( vertices[i].y > y_max )
         y_max = vertices[i].y;
      if( vertices[i].z < z_min )
         z_min = vertices[i].z;
      if( vertices[i].z > z_max )
         z_max = vertices[i].z;
   }
   x_width = x_max - x_min;
   y_width = y_max - y_min;
   z_width = z_max - z_min;
   centroid = vec3(  (x_max-x_min)/2.0+x_min,
                     (y_max-y_min)/2.0+y_min,
                     (z_max-z_min)/2.0+z_min);

   // in case we just want to read the model file
   if ( !load_shader )
      return;

   /// Initialize Shader
   // Create a vertex array object
#ifdef __APPLE__
   glGenVertexArraysAPPLE( 1, &vao );
   glBindVertexArrayAPPLE( vao );
#else
   glGenVertexArrays( 1, &vao );
   glBindVertexArray( vao ); 
#endif

   // Create and initialize a buffer object
   glGenBuffers( 1, &vbo );
   glBindBuffer( GL_ARRAY_BUFFER, vbo );
   glBufferData( GL_ARRAY_BUFFER, 5*vertices_size, NULL, GL_STATIC_DRAW );
   glBufferSubData( GL_ARRAY_BUFFER, 0*vertices_size, vertices_size, &vertices[0]  );
   glBufferSubData( GL_ARRAY_BUFFER, 1*vertices_size, vertices_size, &normals[0]   );
   glBufferSubData( GL_ARRAY_BUFFER, 2*vertices_size, vertices_size, &ambients[0]  );
   glBufferSubData( GL_ARRAY_BUFFER, 3*vertices_size, vertices_size, &diffuses[0]  );
   glBufferSubData( GL_ARRAY_BUFFER, 4*vertices_size, vertices_size, &speculars[0] );
   glBufferSubData( GL_ARRAY_BUFFER, 5*vertices_size, shininess_size, &shininess[0] );

   //load program
   glUseProgram( program );

   // get ids of and set up attribute variables
   GLuint vPosition = glGetAttribLocation( program, "vPosition" );
   glEnableVertexAttribArray( vPosition );
   glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
         BUFFER_OFFSET(0) );

   GLuint vNormal = glGetAttribLocation( program, "vNormal" ); 
   glEnableVertexAttribArray( vNormal );
   glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((1*vertices_size)));

   GLuint vAmbient = glGetAttribLocation( program, "vAmbient" ); 
   glEnableVertexAttribArray( vAmbient );
   glVertexAttribPointer( vAmbient, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((2*vertices_size)));

   GLuint vDiffuse = glGetAttribLocation( program, "vDiffuse" ); 
   glEnableVertexAttribArray( vDiffuse );
   glVertexAttribPointer( vDiffuse, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((3*vertices_size)));

   GLuint vSpecular = glGetAttribLocation( program, "vSpecular" ); 
   glEnableVertexAttribArray( vSpecular );
   glVertexAttribPointer( vSpecular, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((4*vertices_size)));

   GLuint vShiny    = glGetAttribLocation( program, "vShiny" ); 
   glEnableVertexAttribArray( vShiny );
   glVertexAttribPointer( vShiny, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((5*vertices_size)));

   // get ids of uniform variables
   worldview = glGetUniformLocation( program, "worldview" );
   projection = glGetUniformLocation( program, "projection" );
   light_position = glGetUniformLocation( program, "light_position");
   drawmode = glGetUniformLocation( program, "drawmode");
   translation_id = glGetUniformLocation( program, "translation");
   rotation_id = glGetUniformLocation( program, "rotation" );

   translation = vec4(0,0,0,1);
}

void Object::draw_shape( mat4 const& worldviewMat, mat4 const& proj, vec4 const& l_pos ){

   glUseProgram(program);
#ifdef __APPLE__
   glBindVertexArrayAPPLE( vao );
#else
   glBindVertexArray( vao ); 
#endif
   
   glUniform4fv( translation_id, 1, translation );
   glUniformMatrix4fv( rotation_id, 1, GL_TRUE, rotation );
   glUniform4fv( light_position,   1, l_pos );
   glUniformMatrix4fv( projection, 1, GL_TRUE, proj );
   glUniformMatrix4fv( worldview, 1, GL_TRUE, worldviewMat);

   glUniform1i( drawmode, (GLint)TRIANGLES );
   glDrawArrays( GL_TRIANGLES, 0, vertex_count );

#ifdef DEBUG_DRAW_NORMALS
   glUniform1i( drawmode, (GLint)LINES );
   glDrawArrays( GL_LINES, vertex_count, vertex_count*2 );
#endif

}
      
double Object::get_x_width()const{
   return x_width;
}
double Object::get_y_width()const{
   return y_width;
}
double Object::get_z_width()const{
   return z_width;
}

vec3 Object::get_dimensions()const{
   return vec3(get_x_width(), get_y_width(), get_z_width());
}

vec3 Object::get_centroid()const{
   return centroid+get_translation();  
}

void Object::adjust_translation( const vec3& motion ){

   velocity = vec4(motion.x,motion.y,motion.z,1.0);
   
   translation.x += motion.x;
   translation.y += motion.y;
   translation.z += motion.z;
}

void Object::adjust_rotation( const mat4& rotMat ){
   rotation = rotMat*rotation;
}

void Object::set_rotation( const mat4& rotMat ){
   rotation = rotMat;
}

const mat4& Object::get_rotation() const{
   return rotation;
}

vec3 Object::get_translation()const{
   return vec3( translation.x, translation.y, translation.z);
}

const vec4& Object::get_velocity() const{
   return velocity;
}

void Object::set_translation( const vec3& trans ){
   velocity = vec4(trans.x,trans.y,trans.z,0.0) - translation;
   
   translation.x = trans.x;
   translation.y = trans.y;
   translation.z = trans.z;
}

vec4* Object::get_vertices(){
   return &vertices[0];
}

size_t Object::num_verts(){
   return vertex_count;
}
