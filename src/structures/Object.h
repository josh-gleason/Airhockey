#ifndef __SRC_STRUCTURES_OBJECT_H__
#define __SRC_STRUCTURES_OBJECT_H__

#include "vector.h"
#include "mat.h"
#include "GL.h"

#include "objLoader.h"

#include <string>

using namespace std;

class Object{

   public:

      Object( const string& fname, const double& sf, const GLuint prog);
      ~Object();

      void make_empty();
         
      void init_buffers( const vec4& lpos, const vec4& l_amb, const vec4& l_dif, const vec4& l_spec, bool load_shader=true, int rotate=0 );

      void draw_shape( mat4 const& worldviewMat, mat4 const& proj, vec4 const& l_pos );

      double get_x_width()const;
      double get_y_width()const;
      double get_z_width()const;
      vec3 get_dimensions()const;

      vec3 get_centroid()const;

      vec3 get_translation()const;
      void set_translation( const vec3& trans);
      void adjust_translation( const vec3& motion );

      // return const reference so that no copies are made
      const vec4& get_velocity()const;

      vec4* get_vertices();
      size_t num_verts();

   private:
      // defined before drawing so shader knows how to handle vertices
      enum SHAD_DRAW_MODE {TRIANGLES=0, LINES=1};

      void build_arrays( const vec4& lpos, const vec4& l_amb, const vec4& l_dif, const vec4& l_spec );
      
      // attribute types
      GLuint vAngle;
      GLuint Scale;
      GLuint vao;
      GLuint vbo;
      GLuint program;
      GLuint vPosition;
      GLuint vNormal;
      GLuint vAmbient;
      GLuint vDiffuse;
      GLuint vSpecular;
      
      // uniform types
      GLuint translation_id;
      GLuint worldview;
      GLuint projection;
      GLuint light_position;
      GLuint drawmode;
      GLuint rotation_id;

      string model_name;
      objLoader model_data;

      GLuint vertex_count;    /*< Number of vertices in object */
      GLuint vertices_size;   /*< Size of single vertex array */
      GLuint shininess_size;  /*< Size of shininess array */
      
      vec4 * vertices;
      vec4 * normals;
      vec4 * ambients;
      vec4 * diffuses;
      vec4 * speculars;
      GLfloat* shininess;

      vec4 translation;
      GLfloat rotation
      vec4 velocity;

      // no need for scaleing or rotation
      // vec4 rotation
      // vec4 scale

      double x_width;
      double y_width;
      double z_width;
      vec3 centroid;

      double scale_factor; // scale at load
      GLfloat radians;  // rotation at load
};

#endif
