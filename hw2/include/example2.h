#ifndef _glpane_
#define _glpane_
 
#include "wx/wx.h"
#include "wx/glcanvas.h"

//wxDECLARE_EVENT(FILE_RECEIVED, wxCommandEvent);
class BasicGLPane : public wxGLCanvas
{
    wxGLContext*	m_context;
    wxFrame* Parent;
    
public:
	bool starter;

	BasicGLPane(wxFrame* parent, int* args);
	virtual ~BasicGLPane();
 
	void resized(wxSizeEvent& evt);
 
	int getWidth();
	int getHeight();
 
	void render(wxPaintEvent& evt);
	void prepare3DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y);
	void prepare2DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y);
 
	// events
	void InitializeGLEW();
	void mouseMoved(wxMouseEvent& event);
	void mouseDown(wxMouseEvent& event);
	void mouseWheelMoved(wxMouseEvent& event);
	void mouseReleased(wxMouseEvent& event);
	void rightClick(wxMouseEvent& event);
	void mouseLeftWindow(wxMouseEvent& event);
	void keyPressed(wxKeyEvent& event);
	void keyReleased(wxKeyEvent& event);
    void onCharEvent(wxKeyEvent& event);
    void test(wxCommandEvent& event);
    void OnIdle(wxIdleEvent &event);
	DECLARE_EVENT_TABLE()

};


#endif
