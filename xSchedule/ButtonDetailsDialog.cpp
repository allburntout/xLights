#include "ButtonDetailsDialog.h"
#include "xScheduleMain.h"
#include "ScheduleManager.h"

//(*InternalHeaders(ButtonDetailsDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(ButtonDetailsDialog)
const long ButtonDetailsDialog::ID_STATICTEXT1 = wxNewId();
const long ButtonDetailsDialog::ID_TEXTCTRL1 = wxNewId();
const long ButtonDetailsDialog::ID_STATICTEXT2 = wxNewId();
const long ButtonDetailsDialog::ID_CHOICE1 = wxNewId();
const long ButtonDetailsDialog::ID_STATICTEXT3 = wxNewId();
const long ButtonDetailsDialog::ID_TEXTCTRL2 = wxNewId();
const long ButtonDetailsDialog::ID_STATICTEXT4 = wxNewId();
const long ButtonDetailsDialog::ID_CHOICE2 = wxNewId();
const long ButtonDetailsDialog::ID_BUTTON1 = wxNewId();
const long ButtonDetailsDialog::ID_BUTTON2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ButtonDetailsDialog,wxDialog)
	//(*EventTable(ButtonDetailsDialog)
	//*)
END_EVENT_TABLE()

ButtonDetailsDialog::ButtonDetailsDialog(wxWindow* parent, std::string& label, std::string& command, std::string& parameter, char& hotkey, wxWindowID id,const wxPoint& pos,const wxSize& size) : _label(label), _command(command), _parameter(parameter), _hotkey(hotkey)
{
	//(*Initialize(ButtonDetailsDialog)
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer1;

	Create(parent, id, _("Button Details"), wxDefaultPosition, wxDefaultSize, wxCAPTION|wxRESIZE_BORDER|wxMAXIMIZE_BOX, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	FlexGridSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
	FlexGridSizer1->AddGrowableCol(1);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Label:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrl_Label = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	FlexGridSizer1->Add(TextCtrl_Label, 1, wxALL|wxEXPAND, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Command:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer1->Add(StaticText2, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	Choice_Command = new wxChoice(this, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
	FlexGridSizer1->Add(Choice_Command, 1, wxALL|wxEXPAND, 5);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Parameters:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer1->Add(StaticText3, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrl_Parameters = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	FlexGridSizer1->Add(TextCtrl_Parameters, 1, wxALL|wxEXPAND, 5);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Hot Key:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer1->Add(StaticText4, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	Choice_Hotkey = new wxChoice(this, ID_CHOICE2, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));
	FlexGridSizer1->Add(Choice_Hotkey, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer1->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
	Button_Ok = new wxButton(this, ID_BUTTON1, _("Ok"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	Button_Ok->SetDefault();
	FlexGridSizer2->Add(Button_Ok, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button_Cancel = new wxButton(this, ID_BUTTON2, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	FlexGridSizer2->Add(Button_Cancel, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&ButtonDetailsDialog::OnTextCtrl_LabelText);
	Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&ButtonDetailsDialog::OnChoice_CommandSelect);
	Connect(ID_TEXTCTRL2,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&ButtonDetailsDialog::OnTextCtrl_ParametersText);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ButtonDetailsDialog::OnButton_OkClick);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ButtonDetailsDialog::OnButton_CancelClick);
	//*)

    auto commands = xScheduleFrame::GetScheduleManager()->GetCommands();
    for (auto it = commands.begin(); it != commands.end(); ++it)
    {
        Choice_Command->AppendString((*it)->_command);
    }

    TextCtrl_Label->SetValue(label);
    TextCtrl_Parameters->SetValue(parameter);
    Choice_Command->SetSelection(-1);
    for(int i = 0; i < Choice_Command->GetCount(); i++)
    {
        if (Choice_Command->GetString(i) == command)
        {
            Choice_Command->SetSelection(i);
            break;
        }
    }

    for (int i = (int)'A'; i < (int)'z'; i++)
    {
        Choice_Hotkey->AppendString((char)i);
    }

    for (int i = 0; i < Choice_Hotkey->GetCount(); i++)
    {
        if (Choice_Hotkey->GetString(i)[0] == hotkey)
        {
            Choice_Hotkey->SetSelection(i);
            break;
        }
    }

    ValidateWindow();
}

ButtonDetailsDialog::~ButtonDetailsDialog()
{
	//(*Destroy(ButtonDetailsDialog)
	//*)
}


void ButtonDetailsDialog::OnButton_OkClick(wxCommandEvent& event)
{
    _label = TextCtrl_Label->GetValue();
    _parameter = TextCtrl_Parameters->GetValue();
    _command = Choice_Command->GetStringSelection();
    EndDialog(wxID_OK);
}

void ButtonDetailsDialog::OnButton_CancelClick(wxCommandEvent& event)
{
    EndDialog(wxID_CANCEL);
}

void ButtonDetailsDialog::OnTextCtrl_ParametersText(wxCommandEvent& event)
{
    ValidateWindow();
}

void ButtonDetailsDialog::OnTextCtrl_LabelText(wxCommandEvent& event)
{
    ValidateWindow();
}

void ButtonDetailsDialog::OnChoice_CommandSelect(wxCommandEvent& event)
{
    ValidateWindow();
}

void ButtonDetailsDialog::ValidateWindow()
{
    if (TextCtrl_Label->GetValue() == "" || Choice_Command->GetSelection() < 0)
    {
        Button_Ok->Enable(false);
    }
    else
    {
        Button_Ok->Enable(true);
    }
}