


#ifndef _mainframe_
#define _mainframe_
#include "wx/wx.h"
#include "wx/string.h"
#include "wx/menu.h"
#include "wx/textctrl.h"
// Pulled from http://wiki.wxwidgets.org/Writing_Your_First_Application-Common_Dialogs
// MainFrame is the class for our window, it
// contains the window and all objects in it.
static const wxChar *FILETYPES = _T(
	"Text files|*.txt|"
	"C/C++ source files|*.cpp;*.cc;*.c|"
	"C/C++ header files|*.hpp;*.h|"
	"Make files|Mak*;mak*|"
	"Java files|*java|"
	"Hypertext markup files|*html;*htm;*HTML;*HTM|"
	"All files|*.*"
	);
class MainFrame: public wxFrame
{
public:
	MainFrame( const wxString &title, const wxPoint &pos, const wxSize &size );
 
	void Quit( wxCommandEvent& event );
	void NewFile( wxCommandEvent& event );
	void OpenFile( wxCommandEvent& event );
	void SaveFile( wxCommandEvent& event );
	void SaveFileAs( wxCommandEvent& event );
	void CloseFile( wxCommandEvent& event );
 
	//wxTextCtrl *MainEditBox;
	wxMenuBar *MainMenu;
	// The Path to the file we have open
	wxString CurrentDocPath;
 
	DECLARE_EVENT_TABLE()
};
#endif