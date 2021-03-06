#include <GL/glew.h>
#include "wx/wx.h"
#include "wx/sizer.h"
#include "wx/glcanvas.h"

#include <example2.h>

#include <shader.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <gishandler.h>
#include <mainframe.h>


// include OpenGL
#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
//#include <GL/glu.h>
#include <GL/gl.h>
#endif
 
using namespace std;

bool initialize();

/*struct Vertex
{
    GLfloat position[3];
    GLfloat color[3];
};*/
 

int cs=0;
GLuint program;
GLuint vbo_geometry;
GLuint elements;
GLuint texture;
//uniform locations
GLint loc_mvpmat;// Location of the modelviewprojection matrix in the shader
wxImage image;
//attribute locations
GLint loc_position;
GLint loc_color;
GLint shader_status;
GLint loc_height;
GLint colorm;
float max_height = 2;

//transform matrices
glm::mat4 model;//obj->world each object should have its own model matrix
glm::mat4 view;//world->eye
glm::mat4 projection;//eye->clip
glm::mat4 mvp;//premultiplied modelviewprojection
glm::vec3 position = glm::vec3(1.0, 20.0, 1.0);
glm::vec3 direction = {0,0,1.0};
int w = 640, h = 480;
float angle = 0;
float angle2 = 0;

class MyApp: public wxApp
{
    virtual bool OnInit();
 
    wxFrame *frame;
    BasicGLPane * glPane;
    
public:
    
};
 
IMPLEMENT_APP(MyApp)
 
 
bool MyApp::OnInit()
{

    
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    //frame = new wxFrame((wxFrame *)NULL, -1,  wxT("Hello GL World"), wxPoint(50,50), wxSize(400,200));
 
    int args[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};
    
    //glPane = new BasicGLPane( (wxFrame*) frame, args);
    //sizer->Add(glPane, 1,wxEXPAND);
    //glPane->starter = false;
    //frame->SetSizer(sizer);
    //frame->SetAutoLayout(true);

    
    //cout << "Initializing!!" << std::endl;
    //if(!initialize())
     //   return false;
    //frame->Show();

    //glPane->starter = true;

      MainFrame *MainWin = new MainFrame(
    _("Terrain Opener"), wxPoint(1, 1), wxSize(300, 200));
  //MainWin->Show(TRUE); // show the window
  SetTopWindow(MainWin); // and finally, set it as the main window
      glPane = new BasicGLPane( (wxFrame*) MainWin, args);
    sizer->Add(glPane, 1,wxEXPAND);
    //glPane->starter = false;
    MainWin->SetSizer(sizer);
    MainWin->SetAutoLayout(true);
    if(!initialize())
        return false;
  MainWin->Show(true);
  return TRUE;
    //return true;
} 
 




 

// some useful events to use
void BasicGLPane::mouseMoved(wxMouseEvent& event) {}
void BasicGLPane::onCharEvent(wxKeyEvent& event)
{

}
void BasicGLPane::mouseDown(wxMouseEvent& event) {}
void BasicGLPane::mouseWheelMoved(wxMouseEvent& event) {}
void BasicGLPane::mouseReleased(wxMouseEvent& event) {}
void BasicGLPane::rightClick(wxMouseEvent& event) {}
void BasicGLPane::mouseLeftWindow(wxMouseEvent& event) {}

void BasicGLPane::keyPressed(wxKeyEvent& event) 
{

    if(event.GetKeyCode() == wxKeyCode('A'))
    {
      angle += 90;
    }
    else if(event.GetKeyCode() == wxKeyCode('D'))
    {
      angle -= 90;
    }
    if(event.GetKeyCode() == wxKeyCode('W'))
    {
      angle2 += 90;
    }
    else if(event.GetKeyCode() == wxKeyCode('S'))
    {
      angle2 -= 90;
    }
    //std::cout  << " " << angle << "KEY PRESSLED2" << position.x <<  ":POSITION" << position.z << std::endl;
    //    std::cout  << " " << angle << "KEY PRESSLED" << position.x <<  ":POSITION" << position.z << std::endl;



    //direction.rotateY(90.0f);
    direction = glm::vec3(0,0,1);
    direction = glm::rotateY(direction,angle*3.14f/180.f);
    direction = glm::rotateX(direction,angle2*3.14f/180.f);
    glm::vec3 motionvector = glm::vec3(0,0,0);
    if(event.GetKeyCode() == WXK_LEFT)
    {
      position.x -= 1;
    }

    if(event.GetKeyCode() == WXK_RIGHT)
    {
      position.x += 1;
    }

    if(event.GetKeyCode() == WXK_UP)
    {
      direction = glm::normalize(direction);
      motionvector = 2.f*direction;
    }

    if(event.GetKeyCode() == WXK_DOWN)
    {
      direction = glm::normalize(direction);
      motionvector = -2.f * direction;
    }
    position += motionvector;
    view = glm::lookAt( position, //Eye Position
                        glm::vec3(position+direction), //Focus point
                        glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up
    Refresh();
}
void BasicGLPane::keyReleased(wxKeyEvent& event) 
{

}
void BasicGLPane::InitializeGLEW()
{
    //prepare2DViewport(0,0,getWidth(), getHeight());
    // The current canvas has to be set before GLEW can be initialized.
    wxGLCanvas::SetCurrent(*m_context);
    GLenum err = glewInit();

    // If Glew doesn't initialize correctly.
    if(GLEW_OK != err)
    {
        std::cerr << "Error:" << glewGetString(err) << std::endl;
        const GLubyte* String = glewGetErrorString(err);
        wxMessageBox("GLEW is not initialized");
    } 
    glEnable(GL_TEXTURE_1D);
    wxInitAllImageHandlers();
    image.LoadFile("colorMap.png");
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_1D, texture);
    // sample: specify texture parameters
    //glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
 
    /* Linear filtering usually looks best for text */
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
//glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set the active texture

    cout << "IMAGE DIM " << image.GetWidth() << " " << image.GetHeight() << endl;
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, image.GetWidth(), 0, GL_RGB, GL_UNSIGNED_BYTE, 
    image.GetData());
}
// Vertices and faces of a simple cube to demonstrate 3D render
// source: http://www.opengl.org/resources/code/samples/glut_examples/examples/cube.c
GLfloat v[8][3];
GLint faces[6][4] = {  /* Vertex indices for the 6 faces of a cube. */
    {0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
    {4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} };
 
 
 
BasicGLPane::BasicGLPane(wxFrame* parent, int* args) :
    wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
{
	m_context = new wxGLContext(this);
  Parent = parent;
    // prepare a simple cube to demonstrate 3D render
    // source: http://www.opengl.org/resources/code/samples/glut_examples/examples/cube.c
    v[0][0] = v[1][0] = v[2][0] = v[3][0] = -1;
    v[4][0] = v[5][0] = v[6][0] = v[7][0] = 1;
    v[0][1] = v[1][1] = v[4][1] = v[5][1] = -1;
    v[2][1] = v[3][1] = v[6][1] = v[7][1] = 1;
    v[0][2] = v[3][2] = v[4][2] = v[7][2] = 1;
    v[1][2] = v[2][2] = v[5][2] = v[6][2] = -1;    
 
    // To avoid flashing on MSW
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    InitializeGLEW();

    //parent->SetStatusText( "Welcome to wxWidgets!" );
    std::cout << "Creating Context" << getWidth() << " " << getHeight() << std::endl;
    
    
    //cout << "IMAGE DIMENSIONS" << image.GetWidth() << " " << image.GetHeight() << endl; 
    //initialize();
}
 
BasicGLPane::~BasicGLPane()
{
  glDeleteProgram(program);
  glDeleteBuffers(1, &vbo_geometry);
	delete m_context;
}
 
void BasicGLPane::resized(wxSizeEvent& evt)
{
//	wxGLCanvas::OnSize(evt);
  glViewport( 0, 0, getWidth(), getHeight());
            projection = glm::perspective( 45.0f, //the FoV typically 90 degrees is good which is what this is set to
                                   float(getWidth())/float(getHeight()), //Aspect Ratio, so Circles stay Circular
                                   0.01f, //Distance to the near plane, normally a small value like this
                                   100.0f); //Distance to the far plane,

    std::cout << "REFRESHING" << std::endl;  
    Refresh();
}
 
/** Inits the OpenGL viewport for drawing in 3D. */
void BasicGLPane::prepare3DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y)
{
 
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
    glClearDepth(1.0f);	// Depth Buffer Setup
    glEnable(GL_DEPTH_TEST); // Enables Depth Testing
    glDepthFunc(GL_LEQUAL); // The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
 
    glEnable(GL_COLOR_MATERIAL);
 
    glViewport(topleft_x, topleft_y, bottomrigth_x-topleft_x, bottomrigth_y-topleft_y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
 
    float ratio_w_h = (float)(bottomrigth_x-topleft_x)/(float)(bottomrigth_y-topleft_y);
    gluPerspective(45 /*view angle*/, ratio_w_h, 0.1 /*clip close*/, 200 /*clip far*/);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
 
}
 
/** Inits the OpenGL viewport for drawing in 2D. */
void BasicGLPane::prepare2DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
    glEnable(GL_TEXTURE_1D);   // textures
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
 
    glViewport(topleft_x, topleft_y, bottomrigth_x-topleft_x, bottomrigth_y-topleft_y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
 
    gluOrtho2D(topleft_x, bottomrigth_x, bottomrigth_y, topleft_y);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
 
int BasicGLPane::getWidth()
{
    return GetSize().x;
}
 
int BasicGLPane::getHeight()
{
    return GetSize().y;
}
 
 
void BasicGLPane::render( wxPaintEvent& evt )
{
    if(!IsShown()) return;
    //return;
    //if(starter)
    //{
      std::cout << "Rendering" << getWidth() << " " << getHeight() << std::endl;
    wxGLCanvas::SetCurrent(*m_context);
    wxPaintDC(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
 
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //--Render the scene

    //clear the screen
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.2, 1.0);
    //premultiply the matrix for this example
    mvp = projection * view * model;

    //enable the shader program
    glUseProgram(program);
    
    //upload the matrix to the shader
    glUniformMatrix4fv(loc_mvpmat, 1, GL_FALSE, glm::value_ptr(mvp));


    glUniform1f(loc_height,max_height);

    //set up the Vertex Buffer Object so it can be drawn
    glEnableVertexAttribArray(loc_position);
    glEnableVertexAttribArray(loc_color);
    glEnable(GL_TEXTURE_1D);
    glUniform1i(colorm, 0);
 glActiveTexture(GL_TEXTURE0);
 glBindTexture(GL_TEXTURE_1D, texture);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    std::cout << loc_position << " " << loc_color << std::endl;
    //set pointers into the vbo for each of the attributes(position and color)
    glVertexAttribPointer( loc_position,//location of attribute
                           3,//number of elements
                           GL_FLOAT,//type
                           GL_FALSE,//normalized?
                           sizeof(Vertex),//stride
                           0);//offset

    glVertexAttribPointer( loc_color,
                           3,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof(Vertex),
                           (void*)offsetof(Vertex,color));

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements);



 // Draw the triangles !
 glDrawElements(
     GL_LINES,      // mode
     cs,    // count
     GL_UNSIGNED_INT,   // type
     (void*)0           // element array buffer offset
 );
    //return;
    //glDrawArrays(GL_TRIANGLES, 0, 36);//mode, starting index, count

    //clean up
    glDisableVertexAttribArray(loc_position);
    glDisableVertexAttribArray(loc_color);
                           
    //swap the buffers
    //glutSwapBuffers();
    glFlush();
    SwapBuffers();
  //}
}

bool initialize()
{
GLuint vertex_shader;
GLuint frag_shader;

    // A Cube!
    Vertex geometry[] = { {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},

                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},
                          
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},

                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},

                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},

                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          
                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},

                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}}
                        };

    program = glCreateProgram();
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    //shade2 = shader(vertex_shader);
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    //shade = shader(frag_shader);
    shader shade2(vertex_shader);
    shader shade(frag_shader);
    if(!shade.open("../shaders/frag.shader"))
    {
        cout << "Failed to open shader" << std::endl;
        return false;
    }
    if(!shade2.open("../shaders/vert.shader"))
    {
        cout << "Failed to open shader" << endl;
        return false;
    }
    if(!shade.compile())
    {
      cout << "Failed to compile" << endl;
      return false;
    } 
    if(!shade2.compile())
    {
      cout << "failed to compile" << endl;
    } 
    if(!shade2.link(program))
    {
      cout << "failed to link" << endl;
    } 
    if(!shade.link(program))
    {
      cout << "failed to link" << endl;
    } 


    //glTexImage2D(GL_TEXTURE_1D, 0, ship.bytes_per_pixel, ship.width, ship.height, 0, GetDataFormat(), GetDataType(), ship.pixel_data);
    
    //glGetProgramiv(program, GL_LINK_STATUS, &shader_status);
    //if(!shader_status)
    //{
     //   std::cerr << "[F] THE SHADER PROGRAM FAILED TO LINK" << std::endl;
     //   return false;
    //}
       // Create a Vertex Buffer object to store this vertex info on the GPU
    //glGenBuffers(1, &vbo_geometry);
    //glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(geometry), geometry, GL_STATIC_DRAW);
    vector<vector<float>> floatarr = vector<vector<float>>(getRawValuesFromFile("test.pgm"));
    if(floatarr.size() == 0)
    {
      return false;
    }
    cout << "GOT RAW VALUES! " << floatarr.size() << endl;
    float min=100000000,max=-100000;
    for(int i =0; i < floatarr.size(); i++)
      for(int j = 0; j < floatarr[i].size(); j++)
      {
        //cout << "LOOP" << j << " " << floatarr[i].size() << endl;
        float v = floatarr[i][j];
        
        if(v > max)
        {
          max = v;
        }
        if(v < min && v > 0)
        {
          min = v;
        }
        if(v < 0)
        {
          floatarr[i][j] = 0;
        }
      } 
    std::cout << "Creating Mesh" << min << " " << max<< std::endl;
    cs=fromRawCreateMesh(floatarr,min,max,vbo_geometry,elements);
    //Now we set the locations of the attributes and uniforms
    //this allows us to access them easily while rendering
    loc_position = glGetAttribLocation(program,
                    const_cast<const char*>("v_position"));
    if(loc_position == -1)
    {
        std::cerr << "[F] POSITION NOT FOUND" << std::endl;
        return false;
    }

    loc_color = glGetAttribLocation(program,
                    const_cast<const char*>("v_color"));
    if(loc_color == -1)
    {
        std::cerr << "[F] V_COLOR NOT FOUND" << std::endl;
        return false;
    }

    loc_height = glGetUniformLocation(program,
                    const_cast<const char*>("h"));
    if(loc_height == -1)
    {
        std::cerr << "[F] HEIGHT NOT FOUND" << std::endl;
        return false;
    }

    loc_mvpmat = glGetUniformLocation(program,
                    const_cast<const char*>("mvpMatrix"));
    if(loc_mvpmat == -1)
    {
        std::cerr << "[F] MVPMATRIX NOT FOUND" << std::endl;
        return false;
    }

    colorm = glGetUniformLocation(program, "tex");
    if(colorm == -1)
    {
        std::cerr << "[F] Color Map NOT FOUND" << std::endl;
        return false;
    }

    
    std::cout << loc_mvpmat << " " << loc_color << " " << loc_position << std::endl;
    //--Init the view and projection matrices
    //  if you will be having a moving camera the view matrix will need to more dynamic
    //  ...Like you should update it before you render more dynamic 
    //  for this project having them static will be fine
    view = glm::lookAt( glm::vec3(0.0, 5.0, 0.0), //Eye Position
                        glm::vec3(0, 10.0, 1.0), //Focus point
                        glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up
   
   // projection = glm::perspective( 45.0f, //the FoV typically 90 degrees is good which is what this is set to
     //                              float(w)/float(h), //Aspect Ratio, so Circles stay Circular
       //                            0.01f, //Distance to the near plane, normally a small value like this
         //                          100.0f); //Distance to the far plane, 
    
    //enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    return true;
}
