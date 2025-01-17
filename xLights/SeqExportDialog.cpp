#include "SeqExportDialog.h"

//(*InternalHeaders(SeqExportDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/filepicker.h>
#include <wx/config.h>
#include "xLightsMain.h"

//(*IdInit(SeqExportDialog)
const long SeqExportDialog::ID_STATICTEXT1 = wxNewId();
const long SeqExportDialog::ID_CHOICE1 = wxNewId();
const long SeqExportDialog::ID_STATICTEXT3 = wxNewId();
const long SeqExportDialog::ID_TEXTCTRL2 = wxNewId();
const long SeqExportDialog::ID_BUTTON1 = wxNewId();
const long SeqExportDialog::ID_BUTTON2 = wxNewId();
const long SeqExportDialog::ID_BUTTON3 = wxNewId();
//*)

BEGIN_EVENT_TABLE(SeqExportDialog,wxDialog)
    //(*EventTable(SeqExportDialog)
    //*)
END_EVENT_TABLE()

SeqExportDialog::SeqExportDialog(wxWindow* parent, const std::string& model, wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    _model = model;

    //(*Initialize(SeqExportDialog)
    wxFlexGridSizer* FlexGridSizer1;
    wxFlexGridSizer* FlexGridSizer2;
    wxFlexGridSizer* FlexGridSizer3;
    wxFlexGridSizer* FlexGridSizer4;

    Create(parent, wxID_ANY, _("Export Sequence"), wxDefaultPosition, wxDefaultSize, wxCAPTION|wxRESIZE_BORDER, _T("wxID_ANY"));
    SetClientSize(wxSize(385,124));
    FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
    FlexGridSizer1->AddGrowableCol(0);
    FlexGridSizer2 = new wxFlexGridSizer(0, 2, 0, 0);
    FlexGridSizer2->AddGrowableCol(1);
    StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Format"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    FlexGridSizer2->Add(StaticText1, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    ChoiceFormat = new wxChoice(this, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
    ChoiceFormat->SetSelection( ChoiceFormat->Append(_("LOR. *.lms or *.las")) );
    ChoiceFormat->Append(_("Lcb, LOR Clipboard *.lcb"));
    ChoiceFormat->Append(_("Lcb, LOR S5 Clipboard *.lcb"));
    ChoiceFormat->Append(_("Vixen, Vixen sequence file *.vix"));
    ChoiceFormat->Append(_("Vir, Vixen Routine file. *.vir"));
    ChoiceFormat->Append(_("LSP, Light Show Pro "));
    ChoiceFormat->Append(_("HLS, Hinkle Lighte Sequencer *.hlsnc"));
    ChoiceFormat->Append(_("Falcon, *.fseq"));
    ChoiceFormat->Append(_("Compressed Video, *.avi"));
    ChoiceFormat->Append(_("Uncompressed Video, *.avi"));
    ChoiceFormat->Append(_("Minleon Network Effects Controller, *.bin"));
    FlexGridSizer2->Add(ChoiceFormat, 1, wxALL|wxEXPAND, 5);
    StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("File name"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    FlexGridSizer2->Add(StaticText3, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer3 = new wxFlexGridSizer(0, 2, 0, 0);
    FlexGridSizer3->AddGrowableCol(0);
    TextCtrlFilename = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxSize(400,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    FlexGridSizer3->Add(TextCtrlFilename, 1, wxALL|wxEXPAND, 2);
    ButtonFilePick = new wxButton(this, ID_BUTTON1, _("..."), wxDefaultPosition, wxSize(34,-1), 0, wxDefaultValidator, _T("ID_BUTTON1"));
    FlexGridSizer3->Add(ButtonFilePick, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
    FlexGridSizer2->Add(FlexGridSizer3, 1, wxALL|wxEXPAND, 5);
    FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxEXPAND, 5);
    FlexGridSizer4 = new wxFlexGridSizer(0, 3, 0, 0);
    ButtonOk = new wxButton(this, ID_BUTTON2, _("Ok"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    FlexGridSizer4->Add(ButtonOk, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonCancel = new wxButton(this, ID_BUTTON3, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
    FlexGridSizer4->Add(ButtonCancel, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(FlexGridSizer4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(FlexGridSizer1);
    SetSizer(FlexGridSizer1);
    Layout();

    Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&SeqExportDialog::OnChoiceFormatSelect);
    Connect(ID_TEXTCTRL2,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&SeqExportDialog::OnTextCtrlFilenameText);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SeqExportDialog::OnButtonFilePickClick);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SeqExportDialog::OnButtonOkClick);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SeqExportDialog::OnButtonCancelClick);
    //*)

    Fit();

    ButtonOk->SetDefault();
    SetEscapeId(ButtonCancel->GetId());
    ValidateWindow();
}

SeqExportDialog::~SeqExportDialog()
{
    //(*Destroy(SeqExportDialog)
    //*)
}

void SeqExportDialog::ModelExportTypes(bool isgroup)
{
    if (isgroup) {
        ChoiceFormat->Delete(ChoiceFormat->FindString(_("Compressed Video, *.avi")));
        ChoiceFormat->Delete(ChoiceFormat->FindString(_("Uncompressed Video, *.avi")));
        ChoiceFormat->Delete(ChoiceFormat->FindString(_("Minleon Network Effects Controller, *.bin")));
    }
    ChoiceFormat->Delete(ChoiceFormat->FindString(_("LOR. *.lms or *.las")));
    ChoiceFormat->Delete(ChoiceFormat->FindString(_("Vixen, Vixen sequence file *.vix")));
    //ChoiceFormat->Delete(ChoiceFormat->FindString(_("xLights, *.xseq")));
    ChoiceFormat->Delete(ChoiceFormat->FindString(_("Falcon, *.fseq")));
    ChoiceFormat->Append(_("Falcon Pi Sub sequence. *.eseq"));
    ChoiceFormat->Append(_("Falcon Pi Compressed Sub sequence. *.eseq"));

    wxString let;
    wxConfigBase* config = wxConfigBase::Get();
    if (config != nullptr) {
        config->Read("xLightsLastExportType", &let, "");
        if (let == "") {
            ChoiceFormat->SetSelection(0);
        } else {
            if (!ChoiceFormat->SetStringSelection(let)) {
                ChoiceFormat->SetSelection(0);
            }
        }
    } else {
        ChoiceFormat->SetSelection(0);
    }

    SetDefaultName();
}

void SeqExportDialog::SetDefaultName()
{
    wxString fmt = ChoiceFormat->GetStringSelection();
    wxString cwd = xLightsFrame::CurrentDir;
    wxString fsd = ((xLightsFrame*)GetParent())->GetFseqDirectory();
    if (fsd == "") {
        fsd = cwd;
    }

    if (fmt == "LOR. *.lms or *.las") {
        TextCtrlFilename->SetValue(cwd + wxFileName::GetPathSeparator() + _model + ".lms");
    } else if (fmt == "Lcb, LOR Clipboard *.lcb" || fmt == "Lcb, LOR S5 Clipboard *.lcb") {
        TextCtrlFilename->SetValue(cwd + wxFileName::GetPathSeparator() + _model + ".lcb");
    } else if (fmt == "Vixen, Vixen sequence file *.vix") {
        TextCtrlFilename->SetValue(cwd + wxFileName::GetPathSeparator() + _model + ".vix");
    } else if (fmt == "Vir, Vixen Routine file. *.vir") {
        TextCtrlFilename->SetValue(cwd + wxFileName::GetPathSeparator() + _model + ".vir");
    } else if (fmt == "LSP, Light Show Pro ") {
        TextCtrlFilename->SetValue(cwd + wxFileName::GetPathSeparator() + _model);
    } else if (fmt == "HLS, Hinkle Lighte Sequencer *.hlsnc") {
        TextCtrlFilename->SetValue(cwd + wxFileName::GetPathSeparator() + _model + ".hlsnc");
    } else if (fmt == "Falcon Pi Sub sequence. *.eseq") {
        TextCtrlFilename->SetValue(fsd + wxFileName::GetPathSeparator() + _model + ".eseq");
    } else if (fmt == "Falcon Pi Compressed Sub sequence. *.eseq") {
        TextCtrlFilename->SetValue(fsd + wxFileName::GetPathSeparator() + _model + ".eseq");
    } else if (fmt == "Falcon, *.fseq") {
        TextCtrlFilename->SetValue(fsd + wxFileName::GetPathSeparator() + _model + ".fseq");
    } else if (fmt == "Compressed Video, *.avi" || fmt == "Uncompressed Video, *.avi") {
        TextCtrlFilename->SetValue(cwd + wxFileName::GetPathSeparator() + _model + ".avi");
    } else if (fmt == "Minleon Network Effects Controller, *.bin") {
        TextCtrlFilename->SetValue(cwd + wxFileName::GetPathSeparator() + _model + ".bin");
    }
}

void SeqExportDialog::OnChoiceFormatSelect(wxCommandEvent& event)
{
    SetDefaultName();

    TextCtrlFilename->SetFocus();
    ValidateWindow();
}

void SeqExportDialog::OnButtonFilePickClick(wxCommandEvent& event)
{
    wxString fmt = ChoiceFormat->GetStringSelection();
    wxString cwd = xLightsFrame::CurrentDir;
    wxString fsd = ((xLightsFrame*)GetParent())->GetFseqDirectory();
    if (fsd == "") {
        fsd = cwd;
    }

    if (fmt == "LOR. *.lms or *.las") {
        TextCtrlFilename->SetValue(wxFileSelector(_("Choose output file"), cwd, TextCtrlFilename->GetValue(), wxEmptyString, "LOR (*.lms;*.las)|*.lms;*.las", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this));
    } else if (fmt == "Lcb, LOR Clipboard *.lcb" || fmt == "Lcb, LOR S5 Clipboard *.lcb") {
        TextCtrlFilename->SetValue(wxFileSelector(_("Choose output file"), cwd, TextCtrlFilename->GetValue(), wxEmptyString, "LOR Clipboard (*.lcb)|*.lcb", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this));
    } else if (fmt == "Vixen, Vixen sequence file *.vix") {
        TextCtrlFilename->SetValue(wxFileSelector(_("Choose output file"), cwd, TextCtrlFilename->GetValue(), wxEmptyString, "Vixen Sequence File (*.vix)|*.vix", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this));
    } else if (fmt == "Vir, Vixen Routine file. *.vir") {
        TextCtrlFilename->SetValue(wxFileSelector(_("Choose output file"), cwd, TextCtrlFilename->GetValue(), wxEmptyString, "Vixen Routine File (*.vir)|*.vir", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this));
    } else if (fmt == "LSP, Light Show Pro ") {
        TextCtrlFilename->SetValue(wxFileSelector(_("Choose output file"), cwd, TextCtrlFilename->GetValue(), wxEmptyString, "Light Show Pro (*.*)|*.*", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this));
    } else if (fmt == "HLS, Hinkle Lighte Sequencer *.hlsnc") {
        TextCtrlFilename->SetValue(wxFileSelector(_("Choose output file"), cwd, TextCtrlFilename->GetValue(), wxEmptyString, "Hinkle Light Sequencer (*.hlsnc)|*.hlsnc", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this));
    } else if (fmt == "Falcon Pi Sub sequence. *.eseq") {
        TextCtrlFilename->SetValue(wxFileSelector(_("Choose output file"), fsd, TextCtrlFilename->GetValue(), wxEmptyString, "Falcon Sub Sequence (*.eseq)|*.eseq", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this));
    } else if (fmt == "Falcon Pi Compressed Sub sequence. *.eseq") {
        TextCtrlFilename->SetValue(wxFileSelector(_("Choose output file"), fsd, TextCtrlFilename->GetValue(), wxEmptyString, "Falcon Sub Sequence (*.eseq)|*.eseq", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this));
    } else if (fmt == "Falcon, *.fseq") {
        TextCtrlFilename->SetValue(wxFileSelector(_("Choose output file"), fsd, TextCtrlFilename->GetValue(), wxEmptyString, "Falcon (*.fseq)|*.fseq", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this));
    } else if (fmt == "Compressed Video, *.avi" || fmt == "Uncompressed Video, *.avi") {
        TextCtrlFilename->SetValue(wxFileSelector(_("Choose output file"), cwd, TextCtrlFilename->GetValue(), wxEmptyString, "Video (*.avi)|*.avi", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this));
    } else if (fmt == "Minleon Network Effects Controller, *.bin") {
        TextCtrlFilename->SetValue(wxFileSelector(_("Choose output file"), cwd, TextCtrlFilename->GetValue(), wxEmptyString, "Minleon Networks Effects Controller (*.bin)|*.bin", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this));
    }

    ValidateWindow();
}

void SeqExportDialog::OnButtonOkClick(wxCommandEvent& event)
{
    wxConfigBase* config = wxConfigBase::Get();
    if (config != nullptr) {
        config->Write("xLightsLastExportType", ChoiceFormat->GetStringSelection());
    }
    EndDialog(wxID_OK);
}

void SeqExportDialog::OnButtonCancelClick(wxCommandEvent& event)
{
    EndDialog(wxID_CANCEL);
}

void SeqExportDialog::OnTextCtrlFilenameText(wxCommandEvent& event)
{
    ValidateWindow();
}

void SeqExportDialog::ValidateWindow()
{
    if (TextCtrlFilename->GetValue() != "") {
        wxFileName fn(TextCtrlFilename->GetValue());
        if (fn.GetPathWithSep() == "" || wxDir::Exists(fn.GetPathWithSep())) {
            ButtonOk->Enable();
        } else {
            ButtonOk->Disable();
        }
    } else {
        ButtonOk->Disable();
    }
}
