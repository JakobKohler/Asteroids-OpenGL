#include "opengl_renderer.h"
#include <cassert>
#include <span>
#include <utility>
#include "viewer/wavefront.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


// geometric data as in original game and game coordinates
std::vector<Vector2df> spaceship = {
  Vector2df{-6.0f,  3.0f},
  Vector2df{-6.0f, -3.0f},
  Vector2df{-10.0f, -6.0f},
  Vector2df{14.0f,  0.0f},
  Vector2df{-10.0f,  6.0f},
  Vector2df{ -6.0f,  3.0f}
};

std::vector<Vector2df> flame = { 
  Vector2df{-6, 3},
  Vector2df{-12, 0},
  Vector2df{-6, -3}
};

std::vector<Vector2df> torpedo_points = { 
  Vector2df{0, 0},
  Vector2df{0, 1}
};

std::vector<Vector2df> saucer_points = {
  Vector2df{-16, -6},
  Vector2df{16, -6}, 
  Vector2df{40, 6}, 
  Vector2df{-40, 6},
  Vector2df{-16, 18},
  Vector2df{16, 18},
  Vector2df{40, 6},
  Vector2df{16, -6},
  Vector2df{8, -18},
  Vector2df{-8, -18},
  Vector2df{-16, -6},
  Vector2df{-40, 6}
};

std::vector<Vector2df> asteroid_1 = {
  Vector2df{ 0, -12},
  Vector2df{16, -24},
  Vector2df{32, -12},
  Vector2df{24, 0},
  Vector2df{32, 12},
  Vector2df{8, 24}, 
  Vector2df{-16, 24}, 
  Vector2df{-32, 12}, 
  Vector2df{-32, -12}, 
  Vector2df{-16, -24},
  Vector2df{0, -12}
};

std::vector<Vector2df> asteroid_2 = {
  Vector2df{6, -6},
  Vector2df{32, -12},
  Vector2df{16, -24}, 
  Vector2df{0, -16}, 
  Vector2df{-16, -24}, 
  Vector2df{-24, -12},
  Vector2df{-16, -0}, 
  Vector2df{-32, 12}, 
  Vector2df{-16, 24}, 
  Vector2df{-8, 16}, 
  Vector2df{16, 24}, 
  Vector2df{32, 6}, 
  Vector2df{16, -6},
};

std::vector<Vector2df> asteroid_3 = {
  Vector2df{-16, 0}, 
  Vector2df{-32, 6}, 
  Vector2df{-16, 24}, 
  Vector2df{0, 6}, 
  Vector2df{0, 24}, 
  Vector2df{16, 24},
  Vector2df{32, 6}, 
  Vector2df{32, 6}, 
  Vector2df{16, -24}, 
  Vector2df{-8, -24}, 
  Vector2df{-32, -6}
};

std::vector<Vector2df> asteroid_4 = {
  Vector2df{8,0}, 
  Vector2df{32,-6}, 
  Vector2df{32, -12}, 
  Vector2df{8, -24}, 
  Vector2df{-16, -24}, 
  Vector2df{-8, -12}, 
  Vector2df{-32, -12}, 
  Vector2df{-32, 12}, 
  Vector2df{-16, 24}, 
  Vector2df{8, 16}, 
  Vector2df{16, 24}, 
  Vector2df{32, 12}, 
  Vector2df{8, 0}
};

std::vector<Vector2df> spaceship_debris = {
  Vector2df{-2, -1}, 
  Vector2df{-10, 7}, 
  Vector2df{3, 1}, 
  Vector2df{7, 8},
  Vector2df{0, 3}, 
  Vector2df{6, 1},
  Vector2df{3, -1}, 
  Vector2df{ -5, -7},
  Vector2df{0, -4}, 
  Vector2df{-6, -6},
  Vector2df{-2, 2}, 
  Vector2df{2, 5}
};
    
std::vector<Vector2df> spaceship_debris_direction = {
 Vector2df{-40, -23},
 Vector2df{50, 15},
 Vector2df{0, 45},
 Vector2df{60, -15}, 
 Vector2df{10, -52}, 
 Vector2df{-40, 30}
};

std::vector<Vector2df> debris_points = {
 Vector2df{-32, 32}, 
 Vector2df{-32, -16}, 
 Vector2df{-16, 0}, 
 Vector2df{-16, -32}, 
 Vector2df{-8, 24},
 Vector2df{8, -24}, 
 Vector2df{24, 32}, 
 Vector2df{24, -24}, 
 Vector2df{24, -32}, 
 Vector2df{32, -8}
};
        
std::vector<Vector2df> digit_0 = { {0,-8}, {4,-8}, {4,0}, {0,0}, {0, -8} };
std::vector<Vector2df> digit_1 = { {4,0}, {4,-8} };
std::vector<Vector2df> digit_2 = { {0,-8}, {4,-8}, {4,-4}, {0,-4}, {0,0}, {4,0}  };
std::vector<Vector2df> digit_3 = { {0,0}, {4, 0}, {4,-4}, {0,-4}, {4,-4}, {4, -8}, {0, -8}  };
std::vector<Vector2df> digit_4 = { {4,0}, {4,-8}, {4,-4}, {0,-4}, {0,-8}  };
std::vector<Vector2df> digit_5 = { {0,0}, {4,0}, {4,-4}, {0,-4}, {0,-8}, {4, -8}  };
std::vector<Vector2df> digit_6 = { {0,-8}, {0,0}, {4,0}, {4,-4}, {0,-4} };
std::vector<Vector2df> digit_7 = { {0,-8}, {4,-8}, {4,0} };
std::vector<Vector2df> digit_8 = { {0,-8}, {4,-8}, {4,0}, {0,0}, {0,-8}, {0, -4}, {4, -4} };
std::vector<Vector2df> digit_9 = { {4, 0}, {4,-8}, {0,-8}, {0, -4}, {4, -4} };
       
std::vector< std::vector<Vector2df> * > vertice_data = {
  &spaceship,
  &flame,
  &torpedo_points, &saucer_points,
  &asteroid_1, &asteroid_2, &asteroid_3, &asteroid_4,
  &spaceship_debris, &spaceship_debris_direction,
  &debris_points,
  &digit_0, &digit_1, &digit_2, &digit_3, &digit_4, &digit_5, &digit_6, &digit_7, &digit_8, &digit_9 };

// class OpenGLView

  OpenGLView::OpenGLView(GLuint vbo, unsigned int shaderProgram, size_t vertices_size, GLuint mode, bool is_3d_object)
    : shaderProgram(shaderProgram), vertices_size(vertices_size), is_3d(is_3d_object), mode(mode) {

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    if(!is_3d_object){
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }else{
        glVertexAttribPointer(0,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              9 * sizeof(float),
                              (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              9 * sizeof(float),
                              (void*)(6 * sizeof(float)) );
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              9 * sizeof(float),
                              (void*)(3 * sizeof(float)) );
        glEnableVertexAttribArray(2);

    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  OpenGLView::~OpenGLView() {
    glDeleteVertexArrays(1, &vao);
    glDeleteVertexArrays(1, &vao3d);
  }

  void OpenGLView::render( SquareMatrix<float,4> & matrice) {
    glBindVertexArray(vao);
    glUseProgram(shaderProgram);
    int vertex_division_factor = 1;
    if(is_3d){
        vertex_division_factor = 9;
        unsigned int modelLoc = glGetUniformLocation (shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1 , GL_FALSE , &matrice[0][0] ) ;

    }else{
        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &matrice[0][0] );
    }
    glDrawArrays(mode, 0, vertices_size / vertex_division_factor);
  }

// class TypedBodyView

  TypedBodyView::TypedBodyView(TypedBody * typed_body, GLuint vbo, unsigned int shaderProgram, size_t vertices_size, float scale, GLuint mode, bool is_3d,
               std::function<bool()> draw, std::function<void(TypedBodyView *)> modify)
        : OpenGLView(vbo, shaderProgram, vertices_size, mode, is_3d),  typed_body(typed_body), scale(scale), draw(draw), modify(modify) {
  }
  
  SquareMatrix4df TypedBodyView::create_object_transformation(Vector2df direction, float angle, float scale) {
    SquareMatrix4df  translation= { {1.0f,        0.0f,          0.0f, 0.0f},
                                    {0.0f,        1.0f,          0.0f, 0.0f},
                                    {0.0f,        0.0f,          1.0f, 0.0f},
                                    {direction[0], direction[1], 0.0f, 1.0f}
                                  };
    SquareMatrix4df rotation = { { std::cos(angle),  std::sin(angle), 0.0f, 0.0f},
                                 {-std::sin(angle),  std::cos(angle), 0.0f, 0.0f},
                                 { 0.0f,             0.0f,            1.0f, 0.0f},
                                 { 0.0f,             0.0f,            0.0f, 1.0f}
                               };
    SquareMatrix4df  scaling = { { scale,    0.0f, 0.0f,   0.0f},
                                 {  0.0f,   scale, 0.0f,   0.0f},
                                 {  0.0f,    0.0f, scale,  0.0f},
                                 {  0.0f,    0.0f, 0.0f,   1.0f}
                               };                                 

    return translation * rotation * scaling;
  }

  void TypedBodyView::render( SquareMatrix<float,4> & world) {
    debug(2, "render() entry...");
    if ( draw() ) {
      modify(this);
      auto transform = world * create_object_transformation(typed_body->get_position(), typed_body->get_angle(), scale);
      OpenGLView::render(transform);
    }
    debug(2, "render() exit.");
  }
  
 TypedBody * TypedBodyView::get_typed_body() {
   return typed_body;
 }

 bool TypedBodyView::get_is_3d() {
      return is_3d;
  }

 void TypedBodyView::set_scale(float scale) {
   this->scale = scale;
 }

// class OpenGLRenderer
Material default_material = { {1.0f, 1.0f, 1.0f} };

std::vector<float> create_vertices(WavefrontImporter & wi_p) {
  std::vector<float> vertices;
  
  for (Face face : wi_p.get_faces() ) {
    for (ReferenceGroup group : face.reference_groups ) {
      for (size_t i = 0; i < 3; i++) {
        vertices.push_back( group.vertice[i]);
      }
      for (size_t i = 0; i < 3; i++) {
        vertices.push_back( group.normal[i] );
      }
      if (face.material == nullptr) face.material = &default_material;
      for (size_t i = 0; i < 3; i++) {
        vertices.push_back( face.material->ambient[i]);
      }
    } 
  }
  return vertices;
}

void OpenGLRenderer::create3dVbos() {
  size_t vertex_object_count = vertex_data_3d.size();
  vbos3d = new GLuint[vertex_object_count];
  glGenBuffers(vertex_object_count, vbos3d);

  for (size_t i = 0; i < vertex_object_count; i++) {
   glBindBuffer(GL_ARRAY_BUFFER, vbos3d[i]);
   glBufferData(GL_ARRAY_BUFFER, vertex_data_3d[i].size() * sizeof( float ), vertex_data_3d[i].data(), GL_STATIC_DRAW);
 }
}

void OpenGLRenderer::createVbos() {
 vbos = new GLuint[vertice_data.size()];
 glGenBuffers(vertice_data.size(), vbos);

 for (size_t i = 0; i < vertice_data.size(); i++) {
   glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
   glBufferData(GL_ARRAY_BUFFER, vertice_data[i]->size() * sizeof( Vector2df ), vertice_data[i]->data(), GL_STATIC_DRAW);
 }
}

void OpenGLRenderer::create(Spaceship * ship, std::vector< std::unique_ptr<TypedBodyView> > & views) {
  debug(4, "create(Spaceship *) entry...");

  views.push_back(std::make_unique<TypedBodyView>(ship, vbos3d[2], shaderProgram3d, vertex_data_3d[2].size(), 1.0f, GL_TRIANGLES, true,
                  [ship]() -> bool {return ! ship->is_in_hyperspace();}) // only show ship if outside hyperspace
                 );
  views.push_back(std::make_unique<TypedBodyView>(ship, vbos3d[3], shaderProgram3d, vertex_data_3d[3].size(), 1.0f, GL_TRIANGLES, true,
                  [ship]() -> bool {return ! ship->is_in_hyperspace() && ship->is_accelerating();}) // only show flame if accelerating
                 );   

  debug(4, "create(Spaceship *) exit.");
}

void OpenGLRenderer::create(Saucer * saucer, std::vector< std::unique_ptr<TypedBodyView> > & views) {
  debug(4, "create(Saucer *) entry...");
  float scale = 3.0f;
  if ( saucer->get_size() == 0 ) {
    scale = 1.5;
  }
  views.push_back(std::make_unique<TypedBodyView>(saucer, vbos3d[0], shaderProgram3d, vertex_data_3d[0].size(), scale, GL_TRIANGLES, true));
  debug(4, "create(Saucer *) exit.");
}


void OpenGLRenderer::create(Torpedo * torpedo, std::vector< std::unique_ptr<TypedBodyView> > & views) {
  debug(4, "create(Torpedo *) entry...");
  views.push_back(std::make_unique<TypedBodyView>(torpedo, vbos[2], shaderProgram, vertice_data[2]->size(), 1.0f, GL_LINE_LOOP, false));
  debug(4, "create(Torpedo *) exit.");
}

void OpenGLRenderer::create(Asteroid * asteroid, std::vector< std::unique_ptr<TypedBodyView> > & views) {
  float scale = (asteroid->get_size() == 3 ? 1.0 : ( asteroid->get_size() == 2 ? 0.5 : 0.25 ));
  views.push_back(std::make_unique<TypedBodyView>(asteroid, vbos3d[1], shaderProgram3d, vertex_data_3d[1].size(), scale, GL_TRIANGLES, true));
  debug(4, "create(Asteroid *) exit.");
}

void OpenGLRenderer::create(SpaceshipDebris * debris, std::vector< std::unique_ptr<TypedBodyView> > & views) {
  debug(4, "create(SpaceshipDebris *) entry...");
  views.push_back(std::make_unique<TypedBodyView>(debris, vbos[10], shaderProgram, vertice_data[10]->size(), 0.1f, GL_POINTS, false,
            []() -> bool {return true;},
            [debris](TypedBodyView * view) -> void { view->set_scale( 0.5f * (SpaceshipDebris::TIME_TO_DELETE - debris->get_time_to_delete()));}));
  debug(4, "create(SpaceshipDebris *) exit.");
}

void OpenGLRenderer::create(Debris * debris, std::vector< std::unique_ptr<TypedBodyView> > & views) {
  debug(4, "create(Debris *) entry...");
  views.push_back(std::make_unique<TypedBodyView>(debris, vbos[10], shaderProgram, vertice_data[10]->size(), 0.1f, GL_POINTS, false,
            []() -> bool {return true;},
            [debris](TypedBodyView * view) -> void { view->set_scale(Debris::TIME_TO_DELETE - debris->get_time_to_delete());}));   
  debug(4, "create(Debris *) exit.");
}

void OpenGLRenderer::createSpaceShipView() {
  spaceship_view = std::make_unique<OpenGLView>(vbos[0], shaderProgram, vertice_data[0]->size(), GL_LINE_STRIP, false);
}

void OpenGLRenderer::createDigitViews() {
  for (size_t i = 0; i < 10; i++ ) {
    digit_views[i] = std::make_unique<OpenGLView>(vbos[11 + i], shaderProgram, vertice_data[11 + i]->size(), GL_LINE_STRIP, false);
  }
}


void OpenGLRenderer::renderFreeShips(SquareMatrix4df & matrice) {
  constexpr float FREE_SHIP_X = 128;
  constexpr float FREE_SHIP_Y = 64;
  const float PIf = static_cast<float> ( PI );
  Vector2df position = {FREE_SHIP_X, FREE_SHIP_Y};
  SquareMatrix4df rotation = {   { std::cos(-PIf / 2.0f),  std::sin(-PIf / 2.0f), 0.0f, 0.0f},
                                 {-std::sin(-PIf / 2.0f),  std::cos(-PIf / 2.0f), 0.0f, 0.0f},
                                 { 0.0f,                 0.0f,                1.0f, 0.0f},
                                 { 0.0f,                 0.0f,                0.0f, 1.0f}
                               };
  for (int i = 0; i < game.get_no_of_ships(); i++) {
    SquareMatrix4df  translation= { {1.0f,        0.0f,         0.0f, 0.0f},
                                    {0.0f,        1.0f,         0.0f, 0.0f},
                                    {0.0f,        0.0f,         1.0f, 0.0f},
                                    {position[0], position[1],  0.0f, 1.0f} };
    SquareMatrix4df render_matrice = matrice * translation * rotation;
    spaceship_view->render( render_matrice );
    position[0] += 20.0;
  }
}

void OpenGLRenderer::renderScore(SquareMatrix4df & matrice) {
  constexpr float SCORE_X = 128 - 48;
  constexpr float SCORE_Y = 48 - 4;
  

  long long score = game.get_score();
  int no_of_digits = 0;
  if (score > 0) {
    no_of_digits = std::trunc( std::log10( score ) ) + 1;
  }

  Vector2df position = {SCORE_X + 20.0f * no_of_digits,  SCORE_Y};  
  do {
    int d = score % 10;
    score /= 10;
    SquareMatrix4df scale_translation= { {4.0f,        0.0f,         0.0f, 0.0f},
                                         {0.0f,        4.0f,         0.0f, 0.0f},
                                         {0.0f,        0.0f,         1.0f, 0.0f},
                                         {position[0], position[1],  0.0f, 1.0f} };
    SquareMatrix4df render_matrice = matrice * scale_translation;
    digit_views[d]->render( render_matrice );
    no_of_digits--;
    position[0] -= 20;

  } while (no_of_digits > 0);
}


void compile_shader(GLint shader, const char * source) {
  glShaderSource(shader, 1, &source, NULL) ;
  glCompileShader(shader);

  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    std::cerr << " Shader did not compile." << std::endl;
    char log[512];
    glGetShaderInfoLog( shader, 512, NULL, log) ;
    error(log);
    throw EXIT_FAILURE;
  }
}

void OpenGLRenderer::create_3dshader_programs() {

  const char *vertexShaderSource3d = "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "layout (location = 1) in vec3 incolor;\n"
    "layout (location = 2) in vec3 innormal;\n"
    "out vec3 color;\n"
    "out vec4 normal;\n"
    "uniform mat4 model;\n"
    "void main()\n"
    "{\n"
    "gl_Position = model * vec4(position, 1.0);\n"
    "color = incolor;\n"
    "normal = normalize( model * vec4(innormal, 1.0));\n"
    "}\0";

  const char *fragmentShaderSource3d = "#version 330 core\n"
  "out vec4 outColor;\n"
  "in vec3 color;\n"
  "in vec4 normal;\n"
  "void main () {\n"
  "  outColor = vec4(color * (0.3 + 0.7 * max(0.0, dot(normal, normalize( vec4(0.0, 1.0, -4.0, 0.0))))) , 1.0);\n"
  "}\n\0";

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER) ;
  compile_shader(vertexShader, vertexShaderSource3d);

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER) ;
  compile_shader(fragmentShader, fragmentShaderSource3d);

  shaderProgram3d = glCreateProgram() ;
  glAttachShader(shaderProgram3d, vertexShader);
  glAttachShader(shaderProgram3d, fragmentShader);
  glBindFragDataLocation(shaderProgram3d, 0, "outColor");
  glLinkProgram(shaderProgram3d);

  GLint status;
  glGetProgramiv(shaderProgram3d, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    GLint length;
    glGetProgramiv(shaderProgram3d, GL_INFO_LOG_LENGTH, &length);
    GLchar *log = new char[length + 1];
    glGetProgramInfoLog(shaderProgram3d, length, &length, &log[0]);
    error(log);
    throw EXIT_FAILURE;
  }
}

void OpenGLRenderer::create_shader_programs() {

static const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec2 p;\n"
    "uniform mat4 transform;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = transform * vec4(p, 1.0, 1.0);\n"
    "}\0";
static const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
    "}\n\0";

    // build and compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        error( std::string("vertex shader compilation failed") + infoLog);
    }
    // build and compiler fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        error( std::string("fragment shader compilation failed") + infoLog);
    }
    

    // link both shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        error( std::string("linking shader programs failed") + infoLog);
    }
}



bool OpenGLRenderer::init() {
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
    error( std::string("Could not initialize SDL. SDLError: ") + SDL_GetError() );
  } else {
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
    if( window == nullptr ) {
      error( std::string("Could not create Window. SDLError: ") + SDL_GetError() );
    } else {
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG );
      SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
      SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

      context = SDL_GL_CreateContext(window);

      GLenum err = glewInit(); // to be called after OpenGL render context is created
      if (GLEW_OK != err) {
        error( "Could not initialize Glew. Glew error message: " );
        error( glewGetErrorString(err) );
      }
      debug(1, "Using GLEW Version: ");
      debug(1, glewGetString(GLEW_VERSION) );

      SDL_GL_SetSwapInterval(1);

      load_wavefront_data();

      create_shader_programs();
      create_3dshader_programs();
      
      createVbos();
      create3dVbos();
      createSpaceShipView();
      createDigitViews();
      return true;
    }
  }
  return false;
}

/* tile positions in world coordinates
   used to draw objects seemless between boundary
  +---+---+---+   
  | 5 | 7 | 2 |
  +---+---+---+
  | 4 | 0 | 1 |
  +---+---+---+
  | 6 | 8 | 3 |
  +---+---+---+
*/
static Vector2df tile_positions [] = {
                         {0.0f, 0.0f},
                         {1024.0f, 0.0f},
                         {1024.0f, 768.0f},
                         {1024.0f, -768.0f},
                         {-1024.0f, 0.0f},
                         {-1024.0f, 768.0f},
                         {-1024.0f, -768.0f},
                         {0.0f, 768.0f},
                         {0.0f, -768.0f} };

SquareMatrix4df createTranslationMatrix(float x, float y) {
    SquareMatrix4df matrix = {
                    {1, 0, 0, 0},
                    {0, 1, 0, 0},
                    {0, 0, 1, 0},
                    {x, y, 0, 1}
    };
    return matrix;
}

void OpenGLRenderer::render() { //HERE IS THE RENDER, FIRST TRY TO BAKE A RENDER HARD IN HERE //WHERE WILL THE THING BE POSITIONED??
                                //Somehow have to integrate with the existing stuff. How? Different memory layout// Somehow separate 2D and 3D and first render one then switch shader and shit, then render second. Start with hard code 1 3d object tho
  debug(2, "render() entry...");

  // // transformation to canonical view and from left handed to right handed coordinates
  SquareMatrix4df world_transformation =
                         SquareMatrix4df{
                           { 2.0f / 1024.0f,           0.0f,            0.0f,  0.0f},
                           {       0.0f,     -2.0f / 768.0f,            0.0f,  0.0f}, // (negative, because we have a left handed world coord. system)
                           {       0.0f,               0.0f,  2.0f / 1024.0f,  0.0f},
                           {      -1.0f,               1.0f,           -1.0f,  1.0f}
                         };
                                                 
  glClearColor ( 0.0, 0.0, 0.0, 1.0 );
  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  debug(2, "remove views for deleted objects");

  // remove all views for typed bodies that have to be deleted 
  erase_if(views, []( std::unique_ptr<TypedBodyView> & view) { return view->get_typed_body()->is_marked_for_deletion();}); 

  auto new_bodies = game.get_physics().get_recently_added_bodies();
  for (Body2df * body : new_bodies) {
    assert(body != nullptr);
    TypedBody * typed_body = static_cast<TypedBody *>(body);
    auto type = typed_body->get_type();
    if (type == BodyType::spaceship) {
      create( static_cast<Spaceship *>(typed_body), views );
    } else if (type == BodyType::torpedo ) {
      create( static_cast<Torpedo *>(typed_body), views );
    } else  if (type == BodyType::asteroid) {
      create( static_cast<Asteroid *>(typed_body), views );
    } else if (type == BodyType::saucer) {
      create( static_cast<Saucer *>(typed_body), views );
    } else if (type == BodyType::spaceship_debris ) {
      create( static_cast<SpaceshipDebris *>(typed_body), views );
    } else if (type == BodyType::debris) {
      create( static_cast<Debris *>(typed_body), views );
    }
  }

  debug(2, "render all views");
  for (auto & view : views) {
      for (int i = 0; i < 9; ++i) {
          auto resulting_transformation = world_transformation;
          auto shifted_matrix = createTranslationMatrix(tile_positions[i][0], tile_positions[i][1]);
          if(game.ship_exists()) {
              auto ship = game.get_ship();
              Vector2df shipPos = ship->get_position();
              SquareMatrix4df translationShip = SquareMatrix4df{
                      {1.0f,                               0.0f,                                0.0f, 0.0f},
                      {0.0f,                               1.0f,                                0.0f, 0.0f},
                      {0.0f,                               0.0f,                                1.0f, 0.0f},
                      {this->window_width / 2.0f - shipPos[0], this->window_height / 2.0f - shipPos[1], 0.0f, 1.0f}
              };
              resulting_transformation = resulting_transformation * shifted_matrix * translationShip;
          }
          view->render( resulting_transformation );
      }
      //
      //view->render( world_transformation );

  }
  renderFreeShips(world_transformation);
  renderScore(world_transformation);

  SDL_GL_SwapWindow(window);
  debug(2, "render() exit.");
}

void OpenGLRenderer::exit() {
  views.clear();
  glDeleteBuffers(vertice_data.size(), vbos);
  glDeleteBuffers(vertex_data_3d.size(), vbos3d);
  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow( window );
  SDL_Quit();
}

void OpenGLRenderer::load_wavefront_data() {
    vertex_data_3d.clear();

    std::vector<std::string> wavefront_files = {
            "saucer.obj",
            "asteroid.obj",
            "spaceship.obj",
            "spaceship_boost.obj"
    };

    for (const auto& file : wavefront_files) {
        std::vector<float> vertices = load_wavefront_file(file);
        vertex_data_3d.push_back(vertices);
    }
}

std::vector<float> OpenGLRenderer::load_wavefront_file(const std::string &file_path) {
    std::fstream in(file_path);
    WavefrontImporter importer(in);
    importer.parse();
    return create_vertices(importer);
}
 
