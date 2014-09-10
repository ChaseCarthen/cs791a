#include <mainframe.h>
using namespace std;
 enum
{
	// declares an id which will be used to call our button
	TEXT_Main = wxID_HIGHEST + 1,
	MENU_New,
	MENU_Open,
	MENU_Close,
	MENU_Save,
	MENU_SaveAs,
	MENU_Quit
};
wxDEFINE_EVENT(FILE_RECEIVED, wxCommandEvent);
BEGIN_EVENT_TABLE ( MainFrame, wxFrame )
    EVT_COMMAND(wxID_ANY, FILE_RECEIVED, MainFrame::test)
	EVT_MENU(MENU_New, MainFrame::NewFile)
	EVT_MENU(MENU_Open, MainFrame::OpenFile) 
	EVT_MENU(MENU_Close, MainFrame::CloseFile)
	EVT_MENU(MENU_Save, MainFrame::SaveFile)
	EVT_MENU(MENU_SaveAs, MainFrame::SaveFileAs)
	EVT_MENU(MENU_Quit, MainFrame::Quit)
END_EVENT_TABLE()


MainFrame::MainFrame(const wxString &title, const wxPoint &pos, const wxSize &size)
: wxFrame((wxFrame*) NULL, -1, title, pos, size)
{
	CreateStatusBar(2);
 
	MainMenu = new wxMenuBar();
	wxMenu *FileMenu = new wxMenu();
 
	FileMenu->Append(MENU_New,
		_("&New"), _("Create a new file"));
 
	FileMenu->AppendSeparator();
	FileMenu->Append(MENU_Open,
		_("&Open"), _("Open an existing file"));
	FileMenu->Append(MENU_Close,
		_("&Close"), _("Close the current document"));
 
	FileMenu->AppendSeparator();
	FileMenu->Append(MENU_Save,
		_("&Save"), _("Save the current document"));
	FileMenu->Append(MENU_SaveAs,
		_("Save &As"), _("Save the current document under a new file  name"));
 
	FileMenu->AppendSeparator();
	FileMenu->Append(MENU_Quit,
		_("&Quit"), _("Quit the editor"));
 
	MainMenu->Append(FileMenu, _("&File"));
	SetMenuBar(MainMenu);
    panel = NULL;
	/*MainEditBox = new wxTextCtrl(
		this, TEXT_Main, _("Hi!"), wxDefaultPosition, wxDefaultSize, 
		wxTE_MULTILINE | wxTE_RICH , wxDefaultValidator, wxTextCtrlNameStr);*/
 
	Maximize(); // Maximize the window
}

void MainFrame::setGLPane(BasicGLPane* pane)
{
	panel = pane;
}

void MainFrame::test(wxCommandEvent& event )
{
	cout << "THIS IS A TEST: " << event.GetString() << endl;
}
void MainFrame::NewFile(wxCommandEvent& WXUNUSED(event))
{
	// Clear the edit box
	//MainEditBox->Clear();
	// reset the path of our current open file
	CurrentDocPath = wxT("C:/");
	// Set the Title to reflect the file open
	SetTitle(_("Edit - untitled *"));
}
 
void MainFrame::OpenFile(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog *OpenDialog = new wxFileDialog(
		this, _("Choose a file to open"), wxEmptyString, wxEmptyString,FILETYPES,
		wxFD_OPEN, wxDefaultPosition);
 
	// Creates a "open file" dialog with 4 file types
	if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "cancel"
	{
		CurrentDocPath = OpenDialog->GetPath();
        wxCommandEvent event(FILE_RECEIVED); // No specific id
 
        // Add any data; sometimes the only information needed at the destination is the arrival of the event itself
        event.SetString(OpenDialog->GetPath());
 
         // Then post the event
        wxPostEvent(this, event); // to ourselves
        if(panel != NULL)
        {
        	wxPostEvent(panel,event);
        }
		// Sets our current document to the file the user selected
		//MainEditBox->LoadFile(CurrentDocPath); //Opens that file
		// Set the Title to reflect the  file open
		SetTitle(wxString("Edit - ") << OpenDialog->GetFilename());

	}
}
 
void MainFrame::CloseFile(wxCommandEvent& WXUNUSED(event))
{
	// Clear the Text Box
	//MainEditBox->Clear();
	// Reset the current File being edited
	CurrentDocPath = wxT("C:/");
	// Set the Title to reflect the file open
	SetTitle(_("Edit - untitled *"));
}
 
void MainFrame::SaveFile(wxCommandEvent& WXUNUSED(event))
{
	// Save to the already-set path for the document
	//MainEditBox->SaveFile(CurrentDocPath);
}
 
void MainFrame::SaveFileAs(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog *SaveDialog = new wxFileDialog(
		this, _("Save File As _?"), wxEmptyString, wxEmptyString,
		FILETYPES,
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
 
	// Creates a Save Dialog with 4 file types
	if (SaveDialog->ShowModal() == wxID_OK) // If the user clicked "OK"
	{
		CurrentDocPath = SaveDialog->GetPath();
		// set the path of our current document to the file the user chose to save under
		//MainEditBox->SaveFile(CurrentDocPath); // Save the file to the selected path
		// Set the Title to reflect the file open
		SetTitle(wxString("Edit - ") << SaveDialog->GetFilename());
	}
 
	// Clean up after ourselves
	SaveDialog->Destroy();
}
 
void MainFrame::Quit(wxCommandEvent& WXUNUSED(event))
{
	Close(TRUE); // Close the window
}

