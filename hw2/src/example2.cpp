#include <includes.h>

#include <example2.h>
#include <shader.h>
#include <renderer.h>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <mainframe.h>

#include <terrain.h>
#include <terrainmanager.h>
#include <chrono>

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
GLuint texture2;
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
int selected = 0;
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
renderer exampler;
terrain trr;
terrainManager tm;

// Key map
map<int,bool> keymap;
map<int,bool> prevmap;

std::chrono::system_clock::time_point current,previous; 

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
    MainWin->setGLPane(glPane);
    
    if(!initialize())
        return false;
  MainWin->Show(true);
  return TRUE;
    //return true;
} 
 

//wxDEFINE_EVENT(FILE_RECEIVED, wxCommandEvent);
BEGIN_EVENT_TABLE(BasicGLPane, wxGLCanvas)
EVT_IDLE(BasicGLPane::OnIdle)
EVT_MOTION(BasicGLPane::mouseMoved)
EVT_COMMAND(wxID_ANY, FILE_RECEIVED, BasicGLPane::test)
EVT_LEFT_DOWN(BasicGLPane::mouseDown)
EVT_LEFT_UP(BasicGLPane::mouseReleased)
EVT_RIGHT_DOWN(BasicGLPane::rightClick)
EVT_LEAVE_WINDOW(BasicGLPane::mouseLeftWindow)
EVT_SIZE(BasicGLPane::resized)
//EVT_CHAR(BasicGLPane::onCharEvent)
EVT_KEY_DOWN(BasicGLPane::keyPressed)
EVT_KEY_UP(BasicGLPane::keyReleased)
EVT_MOUSEWHEEL(BasicGLPane::mouseWheelMoved)

EVT_PAINT(BasicGLPane::render)

END_EVENT_TABLE()

 
void moveCamera(float dt);
// some useful events to use
void BasicGLPane::test(wxCommandEvent& event)
{
cout << "THIS IS A TEST: " << event.GetString() << endl;
trr.cleanup();
trr.loadTerrain(event.GetString().ToStdString());
trr.createMesh(glm::vec3(1,1,1));
Refresh();
}
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
    keymap[event.GetKeyCode()] = true;
    //cout << "KEY DOWN" << endl;
    
    //Refresh();
    event.Skip();
    
}
void BasicGLPane::keyReleased(wxKeyEvent& event) 
{
    if(keymap[wxKeyCode('[')])
    {
        selected -= 1;
        cout << "Next Image~" << endl;
        selected = selected % tm.getRasterCount();
    }
    if(keymap[wxKeyCode(']')])
    {
        selected += 1;
        cout << "Next Image~" << endl;
        selected = selected % tm.getRasterCount();
    }
    if(keymap[wxKeyCode('1')])
    {
        tm.selectTexture(selected,1);
    }
    if(keymap[wxKeyCode('2')])
    {
        tm.selectTexture(selected,2);
    }
    if(keymap[wxKeyCode('3')])
    {
        tm.selectTexture(selected,3);
    }
    if(keymap[wxKeyCode('4')])
    {
        tm.selectTexture(selected,4);
    }
    if(keymap[wxKeyCode('5')])
    {
        tm.selectTexture(selected,5);
    }
    if(keymap[wxKeyCode('6')])
    {
        tm.selectTexture(selected,6);
    }
    if(keymap[wxKeyCode('7')])
    {
        tm.selectTexture(selected,7);
    }
    if(keymap[wxKeyCode('8')])
    {
        tm.selectTexture(selected,8);
    }
    if(keymap[wxKeyCode('9')])
    {
        tm.selectTexture(selected,9);
    }
    keymap[event.GetKeyCode()] = false;
    cout << "KEY UP" << endl;
}
void BasicGLPane::OnIdle(wxIdleEvent &event)
{
    
    /*for (std::map<int,bool>::iterator it=keymap.begin(); it!=keymap.end(); ++it)
    {
        prevmap[it->first] = it->second;
    }*/
    /*if (wxGetKeyState(wxKeyCode('H'))) {
        cout << "H" << endl;
    }*/
    //cout << "IDLE" << endl;
    current = std::chrono::system_clock::now();
    float dt = std::chrono::duration<float>(current - previous).count(); 
    moveCamera(dt);
    if(dt >= 1.0/60.0)
    {
        //cout << "Rendering" << std::chrono::duration_cast<std::chrono::milliseconds>(current - previous).count() << endl;
        Refresh();
        previous = current;
    }
    else
    {
        //cout << "Not Rendering" << endl;
    }   
    /* Force the redraw immediately, gets the gfx card to its max */
    //event.RequestMore();
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
    //glEnable(GL_TEXTURE_1D);
    glEnable(GL_TEXTURE_2D);
    wxInitAllImageHandlers();
    unsigned char test[64*64];
    for(int i =0; i < 64; i++)
    {
        for(int j = 0; j < 64; j++)
        {
            test[i*64+j] = i+j;
        }
    }

    image.LoadFile("colorMap.png");
    //glGenTextures(1, &texture);
    //glBindTexture(GL_TEXTURE_1D, texture);
    // sample: specify texture parameters
    //glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
 
    /* Linear filtering usually looks best for text */
    //glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
//glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set the active texture

    //cout << "IMAGE DIM " << image.GetWidth() << " " << image.GetHeight() << endl;
    //glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, image.GetWidth(), 0, GL_RGB, GL_UNSIGNED_BYTE, 
    //image.GetData());
    glGenTextures(1,&texture2);
    glBindTexture(GL_TEXTURE_2D,texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 64,64, 0, GL_RED, GL_UNSIGNED_BYTE, test);
    tm.addTerrain("DCEWsqrExtent.tif");
    tm.addTerrain("tl2p5_dem.ipw.tif");
    tm.addShape("streamDCEW/streamDCEW.shp");
    tm.addShape("boundDCEW/boundDCEW.shp");
    tm.selectTerrain(1);
    tm.addMask("tl2p5mask.ipw.tif");
    tm.addRaster("./isnobaloutput/em.1000.tif");
    tm.addRaster("./isnobaloutput/em.1001.tif");
    tm.addRaster("./isnobaloutput/em.1002.tif");
    tm.addRaster("./isnobaloutput/em.1003.tif");
    tm.addRaster("./isnobaloutput/em.1004.tif");
    tm.addRaster("./isnobaloutput/em.1005.tif");
    tm.addRaster("./isnobaloutput/em.1006.tif");
    tm.addRaster("./isnobaloutput/em.1007.tif");
    tm.addRaster("./isnobaloutput/em.1008.tif");
    tm.addRaster("./isnobaloutput/em.1009.tif");
    tm.addRaster("./isnobaloutput/em.1010.tif");
    tm.addRaster("./isnobaloutput/snow.1000.tif");
    tm.addRaster("./isnobaloutput/snow.1001.tif");
    tm.addRaster("./isnobaloutput/snow.1002.tif");
    tm.addRaster("./isnobaloutput/snow.1003.tif");
    tm.addRaster("./isnobaloutput/snow.1004.tif");
    tm.addRaster("./isnobaloutput/snow.1005.tif");
    tm.addRaster("./isnobaloutput/snow.1006.tif");
    tm.addRaster("./isnobaloutput/snow.1007.tif");
    tm.addRaster("./isnobaloutput/snow.1008.tif");
    tm.addRaster("./isnobaloutput/snow.1009.tif");
    tm.addRaster("./isnobaloutput/snow.1010.tif");

    tm.generateGraphMesh(true);
    tm.setActiveTerrain();
    tm.createMeshes();
    tm.selectTexture(0,1);
    //tm.generateGraphMesh(true);
    previous = current = std::chrono::system_clock::now();
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
  exampler.cleanup();
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
                                   1.0f, //Distance to the near plane, normally a small value like this
                                   100000000.0f); //Distance to the far plane,

    std::cout << "REFRESHING" << std::endl;  
    Refresh();
}
 
/** Inits the OpenGL viewport for drawing in 3D. */
void BasicGLPane::prepare3DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y)
{
 
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
    glClearDepth(1.0f);	// Depth Buffer Setup
    glEnable(GL_DEPTH_TEST); // Enables Depth Testing
    glDepthFunc(GL_LESS); // The Type Of Depth Testing To Do
    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_CLIP_VOLUME_CLIPPING_HINT_EXT,GL_FASTEST);
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
      //std::cout << "Rendering" << getWidth() << " " << getHeight() << std::endl;
    wxGLCanvas::SetCurrent(*m_context);
    wxPaintDC(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
 
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //--Render the scene

    //clear the screen
    //tm.render(projection,view,texture);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.2, 1.0);
    //trr.render(projection,view,texture);
    tm.render(projection,view,texture2);
    glFlush();
    //setDoubleBuffer(true);
    //this->SetDoubleBuffered(true);
    SwapBuffers();
  //}
}

bool initialize()
{

    //--Init the view and projection matrices
    //  if you will be having a moving camera the view matrix will need to more dynamic
    //  ...Like you should update it before you render more dynamic 
    //  for this project having them static will be fine
    view = glm::lookAt( glm::vec3(0.0, 5.0, 0.0), //Eye Position
                        glm::vec3(0, 10.0, 1.0), //Focus point
                        glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up
   
    //projection = glm::perspective( 45.0f, //the FoV typically 90 degrees is good which is what this is set to
     //                              float(w)/float(h), //Aspect Ratio, so Circles stay Circular
      //                             0.01f, //Distance to the near plane, normally a small value like this
       //                            100.0f); //Distance to the far plane, 
    
    //enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    keymap[wxKeyCode('A')] = false;
    keymap[wxKeyCode('S')] = false;
    keymap[wxKeyCode('D')] = false;
    keymap[wxKeyCode('W')] = false;
    keymap[WXK_LEFT] = false;
    keymap[WXK_RIGHT] = false;
    keymap[WXK_UP] = false;
    keymap[WXK_DOWN] = false;
    //glEnable(GL_CULL_FACE);
    //glDepthMask( GL_FALSE );
    return true;
}

void moveCamera(float dt)
{

    if(keymap[wxKeyCode('A')] )
    {
        cout << "ANGLE" << endl;
        angle += 15*dt;
    }
    if(keymap[wxKeyCode('D')] )
    {
        angle -= 15*dt;
    }
    if(keymap[wxKeyCode('W')] )
    {
        angle2 += 15*dt;
    }
    if(keymap[wxKeyCode('S')] )
    {
        angle2 -= 15*dt;
    }

    direction = glm::vec3(0,0,1);
    //direction = glm::rotateY(direction,angle);
    //direction = glm::rotateX(direction,angle2);
    direction = glm::rotate(direction, angle, glm::vec3(0,1,0));
    direction = glm::rotate(direction, angle2, glm::cross(direction, glm::vec3(0,1,0)));
    glm::vec3 motionvector = glm::vec3(0,0,0);
    if(keymap[WXK_LEFT] )
    {
        motionvector += 2.f*glm::cross(direction, glm::vec3(0,1,0));

    }

    if(keymap[WXK_RIGHT] )
    {
        motionvector -= 2.f*glm::cross(direction, glm::vec3(0,1,0));
    }

    if(keymap[WXK_UP] )
    {
        direction = glm::normalize(direction);
        motionvector += 2.f*direction;
    }

    if(keymap[WXK_DOWN])
    {
        direction = glm::normalize(direction);
        motionvector += -2.f * direction;
    }
    position += motionvector;
    view = glm::lookAt( position, //Eye Position
                        glm::vec3(position+direction), //Focus point
                        glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up
}
