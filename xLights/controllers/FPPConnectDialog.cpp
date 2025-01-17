//(*InternalHeaders(FPPConnectDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <wx/regex.h>
#include <wx/volume.h>
#include <wx/progdlg.h>
#include <wx/config.h>
#include <wx/dir.h>

#include "FPPConnectDialog.h"
#include "xLightsMain.h"
#include "FPP.h"
#include "xLightsXmlFile.h"
#include "outputs/Output.h"
#include "outputs/OutputManager.h"
#include "UtilFunctions.h"

#include <log4cpp/Category.hh>
#include "../xSchedule/wxJSON/jsonreader.h"

#include "../include/spxml-0.5/spxmlparser.hpp"
#include "../include/spxml-0.5/spxmlevent.hpp"
#include "../FSEQFile.h"
#include "../Parallel.h"

//(*IdInit(FPPConnectDialog)
const long FPPConnectDialog::ID_SCROLLEDWINDOW1 = wxNewId();
const long FPPConnectDialog::ID_LISTVIEW_Sequences = wxNewId();
const long FPPConnectDialog::ID_SPLITTERWINDOW1 = wxNewId();
const long FPPConnectDialog::ID_BUTTON1 = wxNewId();
const long FPPConnectDialog::ID_BUTTON_Upload = wxNewId();
//*)

const long FPPConnectDialog::ID_MNU_SELECTALL = wxNewId();
const long FPPConnectDialog::ID_MNU_SELECTNONE = wxNewId();
const long FPPConnectDialog::ID_FPP_INSTANCE_LIST = wxNewId();

BEGIN_EVENT_TABLE(FPPConnectDialog,wxDialog)
	//(*EventTable(FPPConnectDialog)
	//*)
END_EVENT_TABLE()


static const std::string CHECK_COL = "ID_UPLOAD_";
static const std::string FSEQ_COL = "ID_FSEQTYPE_";
static const std::string MEDIA_COL = "ID_MEDIA_";
static const std::string MODELS_COL = "ID_MODELS_";
static const std::string UDP_COL = "ID_UDPOUT_";
static const std::string PLAYLIST_COL = "ID_PLAYLIST_";
static const std::string UPLOAD_CONTROLLER_COL = "ID_CONTROLLER_";


FPPConnectDialog::FPPConnectDialog(wxWindow* parent, OutputManager* outputManager, wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    _outputManager = outputManager;

	//(*Initialize(FPPConnectDialog)
	wxButton* cancelButton;
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer4;

	Create(parent, wxID_ANY, _("FPP Upload"), wxDefaultPosition, wxDefaultSize, wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxCLOSE_BOX|wxMAXIMIZE_BOX, _T("wxID_ANY"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(0);
	SplitterWindow1 = new wxSplitterWindow(this, ID_SPLITTERWINDOW1, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_3DSASH, _T("ID_SPLITTERWINDOW1"));
	SplitterWindow1->SetMinSize(wxDLG_UNIT(this,wxSize(550,-1)));
	SplitterWindow1->SetMinimumPaneSize(200);
	SplitterWindow1->SetSashGravity(0.5);
	FPPInstanceList = new wxScrolledWindow(SplitterWindow1, ID_SCROLLEDWINDOW1, wxDefaultPosition, wxDefaultSize, wxVSCROLL|wxHSCROLL, _T("ID_SCROLLEDWINDOW1"));
	FPPInstanceList->SetMinSize(wxDLG_UNIT(SplitterWindow1,wxSize(-1,150)));
	FPPInstanceSizer = new wxFlexGridSizer(0, 10, 0, 0);
	FPPInstanceList->SetSizer(FPPInstanceSizer);
	FPPInstanceSizer->Fit(FPPInstanceList);
	FPPInstanceSizer->SetSizeHints(FPPInstanceList);
	CheckListBox_Sequences = new wxListView(SplitterWindow1, ID_LISTVIEW_Sequences, wxDefaultPosition, wxDefaultSize, wxLC_REPORT, wxDefaultValidator, _T("ID_LISTVIEW_Sequences"));
	CheckListBox_Sequences->SetMinSize(wxDLG_UNIT(SplitterWindow1,wxSize(-1,100)));
	SplitterWindow1->SplitHorizontally(FPPInstanceList, CheckListBox_Sequences);
	FlexGridSizer1->Add(SplitterWindow1, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer4 = new wxFlexGridSizer(0, 5, 0, 0);
	AddFPPButton = new wxButton(this, ID_BUTTON1, _("Add FPP"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	FlexGridSizer4->Add(AddFPPButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer4->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	wxSize __SpacerSize_1 = wxDLG_UNIT(this,wxSize(50,-1));
	FlexGridSizer4->Add(__SpacerSize_1.GetWidth(),__SpacerSize_1.GetHeight(),1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button_Upload = new wxButton(this, ID_BUTTON_Upload, _("Upload"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_Upload"));
	FlexGridSizer4->Add(Button_Upload, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	cancelButton = new wxButton(this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_CANCEL"));
	FlexGridSizer4->Add(cancelButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(FlexGridSizer4, 1, wxALL|wxEXPAND, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_LISTVIEW_Sequences,wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK,(wxObjectEventFunction)&FPPConnectDialog::SequenceListPopup);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&FPPConnectDialog::OnAddFPPButtonClick);
	Connect(ID_BUTTON_Upload,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&FPPConnectDialog::OnButton_UploadClick);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&FPPConnectDialog::OnClose);
	//*)

    CheckListBox_Sequences->EnableCheckBoxes();

    wxProgressDialog prgs("Discovering FPP Instances",
                          "Discovering FPP Instances", 100, parent);
    prgs.Pulse("Discovering FPP Instances");
    prgs.Show();
    

    std::list<std::string> startAddresses;

    wxConfigBase* config = wxConfigBase::Get();
    wxString force;
    if (config->Read("FPPConnectForcedIPs", &force)) {
        wxArrayString ips = wxSplit(force, '|');
        wxString newForce;
        for (auto &a : ips) {
            startAddresses.push_back(a);
        }
    }
    FPP::Discover(startAddresses, instances);
    wxString newForce = "";
    for (auto &a : startAddresses) {
        for (auto fpp : instances) {
            if (a == fpp->hostName || a == fpp->ipAddress) {
                if (newForce != "") {
                    newForce.append(",");
                }
                newForce.append(a);
            }
        }
    }
    if (newForce != force) {
        config->Write("FPPConnectForcedIPs", newForce);
    }
    
    prgs.Pulse("Checking for mounted media drives");
    CreateDriveList();
    prgs.Update(100);
    prgs.Hide();


    SetSizer(FlexGridSizer1);
    FlexGridSizer1->Fit(this);
    FlexGridSizer1->SetSizeHints(this);
    
    AddInstanceHeader("Upload");
    AddInstanceHeader("Location");
    AddInstanceHeader("Description");
    AddInstanceHeader("Version");
    AddInstanceHeader("FSEQ Type");
    AddInstanceHeader("Media");
    AddInstanceHeader("Models");
    AddInstanceHeader("UDP Out");
    AddInstanceHeader("Playlist");
    AddInstanceHeader("Pixel Hat/Cape");
    
    PopulateFPPInstanceList();
    LoadSequences();
    
    int h = SplitterWindow1->GetSize().GetHeight();
    h *= 33;
    h /= 100;
    SplitterWindow1->SetSashPosition(h);
}

void FPPConnectDialog::PopulateFPPInstanceList() {
    FPPInstanceList->Freeze();
    //remove all the children from the first upload checkbox on
    wxWindow *w = FPPInstanceList->FindWindow(CHECK_COL + "0");
    while (w) {
        wxWindow *tmp = w->GetNextSibling();
        w->Destroy();
        w = tmp;
    }
    while (FPPInstanceSizer->GetItemCount () > 10) {
        FPPInstanceSizer->Remove(10);
    }

    int row = 0;
    for (auto inst : instances) {
        std::string rowStr = std::to_string(row);
        wxCheckBox *CheckBox1 = new wxCheckBox(FPPInstanceList, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, CHECK_COL + rowStr);
        CheckBox1->SetValue(true);
        FPPInstanceSizer->Add(CheckBox1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
        std::string l = inst->hostName + " - " + inst->ipAddress;
        wxStaticText *label = new wxStaticText(FPPInstanceList, wxID_ANY, l, wxDefaultPosition, wxDefaultSize, 0, _T("ID_LOCATION_" + rowStr));
        FPPInstanceSizer->Add(label, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
        label = new wxStaticText(FPPInstanceList, wxID_ANY, inst->description, wxDefaultPosition, wxDefaultSize, 0, _T("ID_DESCRIPTION_" + rowStr));
        FPPInstanceSizer->Add(label, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
        label = new wxStaticText(FPPInstanceList, wxID_ANY, inst->fullVersion, wxDefaultPosition, wxDefaultSize, 0, _T("ID_VERSION_" + rowStr));
        FPPInstanceSizer->Add(label, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
        
        //FSEQ Type listbox
        if (inst->IsVersionAtLeast(2, 6)) {
            wxChoice *Choice1 = new wxChoice(FPPInstanceList, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, FSEQ_COL + rowStr);
            wxFont font = Choice1->GetFont();
            font.SetPointSize(font.GetPointSize() - 2);
            Choice1->SetFont(font);
            Choice1->Append(_("V1"));
            Choice1->Append(_("V2"));
            Choice1->Append(_("V2 Sparse"));
            Choice1->SetSelection(inst->mode == "master" ? 1 : 2);
            FPPInstanceSizer->Add(Choice1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
        } else {
            label = new wxStaticText(FPPInstanceList, wxID_ANY, "V1", wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATIC_TEXT_FS_" + rowStr));
            FPPInstanceSizer->Add(label, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
        }

        CheckBox1 = new wxCheckBox(FPPInstanceList, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, MEDIA_COL + rowStr);
        CheckBox1->SetValue(inst->mode != "remote");
        FPPInstanceSizer->Add(CheckBox1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
        if (inst->IsVersionAtLeast(2, 6)) {
            CheckBox1 = new wxCheckBox(FPPInstanceList, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, MODELS_COL + rowStr);
            CheckBox1->SetValue(false);
            FPPInstanceSizer->Add(CheckBox1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
        } else {
            FPPInstanceSizer->Add(0,0,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
        }
        if (inst->IsVersionAtLeast(2, 0)) {
            CheckBox1 = new wxCheckBox(FPPInstanceList, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, UDP_COL + rowStr);
            CheckBox1->SetValue(false);
            FPPInstanceSizer->Add(CheckBox1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
        } else {
            FPPInstanceSizer->Add(0,0,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
        }

        //playlist combo box
        if (inst->IsVersionAtLeast(2, 6)) {
            wxComboBox *ComboBox1 = new wxComboBox(FPPInstanceList, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, PLAYLIST_COL + rowStr);
            std::list<std::string> playlists;
            inst->LoadPlaylists(playlists);
            ComboBox1->Append(_(""));
            for (auto pl : playlists) {
                ComboBox1->Append(pl);
            }
            wxFont font = ComboBox1->GetFont();
            font.SetPointSize(font.GetPointSize() - 2);
            ComboBox1->SetFont(font);
            FPPInstanceSizer->Add(ComboBox1, 1, wxALL|wxEXPAND, 0);
        } else {
            FPPInstanceSizer->Add(0,0,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
        }

        if (inst->PixelContollerDescription() != "") {
            std::string desc = inst->PixelContollerDescription();
            if (inst->panelSize != "") {
                desc += " - " + inst->panelSize;
            }
            CheckBox1 = new wxCheckBox(FPPInstanceList, wxID_ANY, desc, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, UPLOAD_CONTROLLER_COL + rowStr);
            CheckBox1->SetValue(false);
            FPPInstanceSizer->Add(CheckBox1, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
        } else {
            FPPInstanceSizer->Add(0,0,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
        }

        row++;
    }
    ApplySavedHostSettings();
    
    FPPInstanceList->FitInside();
    FPPInstanceList->SetScrollRate(10, 10);
    FPPInstanceList->ShowScrollbars(wxSHOW_SB_ALWAYS, wxSHOW_SB_ALWAYS);
    FPPInstanceList->Thaw();
}

void FPPConnectDialog::AddInstanceHeader(const std::string &h) {
    wxPanel *Panel1 = new wxPanel(FPPInstanceList, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    wxBoxSizer *BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText *StaticText3 = new wxStaticText(Panel1, wxID_ANY, h, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    BoxSizer1->Add(StaticText3, 1, wxLEFT|wxRIGHT|wxEXPAND, 5);
    Panel1->SetSizer(BoxSizer1);
    BoxSizer1->Fit(Panel1);
    BoxSizer1->SetSizeHints(Panel1);
    FPPInstanceSizer->Add(Panel1, 1, wxALL|wxEXPAND, 0);
}

void FPPConnectDialog::AddInstanceRow(const FPP &inst) {
    
}

void FPPConnectDialog::OnPopup(wxCommandEvent &event)
{
    int id = event.GetId();
    if (id == ID_MNU_SELECTALL) {
        for (size_t i = 0; i < CheckListBox_Sequences->GetItemCount(); i++) {
            if (!CheckListBox_Sequences->IsItemChecked(i)) {
                CheckListBox_Sequences->CheckItem(i, true);
            }
        }
    } else if (id == ID_MNU_SELECTNONE) {
        for (size_t i = 0; i < CheckListBox_Sequences->GetItemCount(); i++) {
            if (CheckListBox_Sequences->IsItemChecked(i)) {
                CheckListBox_Sequences->CheckItem(i, false);
            }
        }
    }
}

FPPConnectDialog::~FPPConnectDialog()
{
	//(*Destroy(FPPConnectDialog)
	//*)
    
    for (auto a : instances) {
        delete a;
    }
}

void FPPConnectDialog::LoadSequencesFromFolder(wxString dir) const
{
    wxLogNull logNo; //kludge: avoid "error 0" message from wxWidgets
    static log4cpp::Category &logger_base = log4cpp::Category::getInstance(std::string("log_base"));
    logger_base.info("Scanning folder for sequences for FPP upload: %s", (const char *)dir.c_str());

    const wxString fseqDir = xLightsFrame::FseqDir;

    wxDir directory;
    directory.Open(dir);

    wxString file;
    bool fcont = directory.GetFirst(&file, "*.xml");
    static const int BUFFER_SIZE = 1024*12;
    std::vector<char> buf(BUFFER_SIZE); //12K buffer
    while (fcont) {
        if (file != "xlights_rgbeffects.xml" && file != OutputManager::GetNetworksFileName() && file != "xlights_keybindings.xml") {
            // this could be a sequence file ... lets open it and check
            //just check if <xsequence" is in the first 512 bytes, parsing every XML is way too expensive
            wxFile doc(dir + wxFileName::GetPathSeparator() + file);
            SP_XmlPullParser *parser = new SP_XmlPullParser();
            size_t read = doc.Read(&buf[0], BUFFER_SIZE);
            parser->append(&buf[0], read);
            SP_XmlPullEvent * event = parser->getNext();
            int done = 0;
            int count = 0;
            bool isSequence = false;
            bool isMedia = false;
            std::string mediaName;


            while (!done) {
                if (!event) {
                    size_t read2 = doc.Read(&buf[0], BUFFER_SIZE);
                    if (read2 == 0) {
                        done = true;
                    } else {
                        parser->append(&buf[0], read2);
                    }
                } else {
                    switch (event->getEventType()) {
                        case SP_XmlPullEvent::eEndDocument:
                            done = true;
                            break;
                        case SP_XmlPullEvent::eStartTag: {
                                SP_XmlStartTagEvent * stagEvent = (SP_XmlStartTagEvent*)event;
                                wxString NodeName = wxString::FromAscii(stagEvent->getName());
                                count++;
                                if (NodeName == "xsequence") {
                                    isSequence = true;
                                } else if (NodeName == "mediaFile") {
                                    isMedia = true;
                                } else {
                                    isMedia = false;
                                }
                                if (count == 100) {
                                    //media file will be very early in the file, dont waste time;
                                    done = true;
                                }
                            }
                            break;
                        case SP_XmlPullEvent::eCData:
                            if (isMedia) {
                                SP_XmlCDataEvent * stagEvent = (SP_XmlCDataEvent*)event;
                                mediaName = wxString::FromAscii(stagEvent->getText()).ToStdString();
                                done = true;
                            }
                            break;
                    }
                }
                if (!done) {
                    event = parser->getNext();
                }
            }
            delete parser;

            xLightsFrame* frame = static_cast<xLightsFrame*>(GetParent());

            std::string fseqName = frame->GetFseqDirectory() + wxFileName::GetPathSeparator() + file.substr(0, file.length() - 4) + ".fseq";
            if (isSequence) {
                //need to check for existence of fseq
                if (!wxFile::Exists(fseqName)) {
                    fseqName = dir + wxFileName::GetPathSeparator() + file.substr(0, file.length() - 4) + ".fseq";
                }
                if (!wxFile::Exists(fseqName)) {
                    isSequence = false;
                }
            }
            if (mediaName != "") {
                if (!wxFile::Exists(mediaName)) {
                    wxFileName fn(mediaName);
                    if (wxFile::Exists(frame->mediaDirectory + wxFileName::GetPathSeparator() + fn.GetFullName())) {
                        mediaName = frame->mediaDirectory + wxFileName::GetPathSeparator() + fn.GetFullName();
                    } else {
                        mediaName = "";
                    }
                }
            }
            
            if (isSequence) {
                long index = CheckListBox_Sequences->GetItemCount();
                CheckListBox_Sequences->InsertItem(index, fseqName);
                if (mediaName != "") {
                    CheckListBox_Sequences->SetItem(index, 1, mediaName);
                }
            }
        }
        fcont = directory.GetNext(&file);
    }

    fcont = directory.GetFirst(&file, wxEmptyString, wxDIR_DIRS);
    while (fcont)
    {
        if (file != "Backup")
        {
            LoadSequencesFromFolder(dir + wxFileName::GetPathSeparator() + file);
        }
        fcont = directory.GetNext(&file);
    }
}

void FPPConnectDialog::LoadSequences()
{
    CheckListBox_Sequences->ClearAll();
    CheckListBox_Sequences->AppendColumn("Sequence");
    CheckListBox_Sequences->AppendColumn("Media");
    LoadSequencesFromFolder(xLightsFrame::CurrentDir);
    
    xLightsFrame* frame = static_cast<xLightsFrame*>(GetParent());
    wxDir directory;
    directory.Open(frame->GetFseqDirectory());

    wxString file;
    bool fcont = directory.GetFirst(&file, "*.?seq");
    while (fcont) {
        int i = CheckListBox_Sequences->FindItem(0, frame->GetFseqDirectory() + wxFileName::GetPathSeparator() + file, true);
        if (i == -1) {
            wxListItem info;
            info.SetText(frame->GetFseqDirectory() + wxFileName::GetPathSeparator() + file);
            info.SetId(99999);
            CheckListBox_Sequences->InsertItem(info);
        }
        fcont = directory.GetNext(&file);
    }

    if (xLightsFrame::CurrentSeqXmlFile != nullptr) {
        wxString curSeq = xLightsFrame::CurrentSeqXmlFile->GetLongPath();
        if (!curSeq.StartsWith(xLightsFrame::CurrentDir)) {
            LoadSequencesFromFolder(xLightsFrame::CurrentSeqXmlFile->GetLongPath());
        }
        int i = CheckListBox_Sequences->FindItem(0, xLightsFrame::CurrentSeqXmlFile->GetLongPath(), true);
        if (i != -1) {
            CheckListBox_Sequences->CheckItem(i, true);
        }
    }
    
    wxConfigBase* config = wxConfigBase::Get();
    if (config != nullptr) {
        const wxString itcsv = config->Read("FPPConnectSelectedSequences", wxEmptyString);
        
        if (!itcsv.IsEmpty()) {
            wxArrayString savedUploadItems = wxSplit(itcsv, ',');
            
            for (int x = 0; x < CheckListBox_Sequences->GetItemCount(); x++) {
                if (savedUploadItems.Index(CheckListBox_Sequences->GetItemText(x)) != wxNOT_FOUND) {
                    CheckListBox_Sequences->CheckItem(x, true);
                }
            }
        }
    }

    CheckListBox_Sequences->SetColumnWidth(0, wxLIST_AUTOSIZE);
    CheckListBox_Sequences->SetColumnWidth(1, wxLIST_AUTOSIZE);
}

void FPPConnectDialog::OnButton_UploadClick(wxCommandEvent& event)
{
    static log4cpp::Category& logger_base = log4cpp::Category::getInstance(std::string("log_base"));
    bool cancelled = false;
    
    std::vector<bool> doUpload(instances.size());
    int row = 0;
    for (row = 0; row < doUpload.size(); ++row) {
        std::string rowStr = std::to_string(row);
        doUpload[row] = GetCheckValue(CHECK_COL + rowStr);
    }
    for (int fs = 0; fs < CheckListBox_Sequences->GetItemCount(); fs++) {
        if (CheckListBox_Sequences->IsItemChecked(fs)) {
            std::string fseq = CheckListBox_Sequences->GetItemText(fs);
            std::string media = CheckListBox_Sequences->GetItemText(fs, 1);

            FSEQFile *seq = FSEQFile::openFSEQFile(fseq);
            if (seq) {
                row = 0;
                for (auto &inst : instances) {
                    std::string rowStr = std::to_string(row);
                    if (!cancelled && doUpload[row]) {
                        inst->parent = this;
                        std::string m2 = media;
                        if (!GetCheckValue(MEDIA_COL + rowStr)) {
                            m2 = "";
                        }

                        int fseqType = GetChoiceValueIndex(FSEQ_COL + rowStr);
                        cancelled |= inst->PrepareUploadSequence(*seq,
                                                                fseq, m2,
                                                                fseqType);
                    }
                    row++;
                }
                if (!cancelled) {
                    wxProgressDialog prgs("Generating FSEQ Files", "Generating " + fseq, 1000, this, wxPD_CAN_ABORT | wxPD_APP_MODAL);
                    prgs.Show();
                    int lastDone = 0;
                    static const int FRAMES_TO_BUFFER = 50;
                    std::vector<std::vector<uint8_t>> frames(FRAMES_TO_BUFFER);
                    for (int x = 0; x < frames.size(); x++) {
                        frames[x].resize(seq->getMaxChannel() + 1);
                    }
                    
                    for (int frame = 0; frame < seq->getNumFrames() && !cancelled; frame++) {
                        int donePct = frame * 1000 / seq->getNumFrames();
                        if (donePct != lastDone) {
                            lastDone = donePct;
                            cancelled |= !prgs.Update(donePct, "Generating " + fseq, &cancelled);
                            wxYield();
                        }

                        int lastBuffered = 0;
                        int startFrame = frame;
                        //Read a bunch of frames so each parallel thread has more info to work with before returning out here
                        while (lastBuffered < FRAMES_TO_BUFFER && frame < seq->getNumFrames()) {
                            FSEQFile::FrameData *f = seq->getFrame(frame);
                            if (f != nullptr)
                            {
                                if (!f->readFrame(&frames[lastBuffered][0], frames[lastBuffered].size()))
                                {
                                    logger_base.error("FPPConnect FSEQ file corrupt.");
                                }
                                delete f;
                            }
                            lastBuffered++;
                            frame++;
                        }
                        frame--;
                        std::function<void(FPP * &, int)> func = [startFrame, lastBuffered, &frames, &doUpload](FPP* &inst, int row) {
                            if (doUpload[row]) {
                                for (int x = 0; x < lastBuffered; x++) {
                                    inst->AddFrameToUpload(startFrame + x, &frames[x][0]);
                                }
                            }
                        };
                        parallel_for(instances, func);
                    }
                    prgs.Hide();
                }
                row = 0;
                for (auto &inst : instances) {
                    if (!cancelled && doUpload[row]) {
                        cancelled |= inst->FinalizeUploadSequence();
                    }
                    row++;
                }
            }
            delete seq;
        }
    }
    row = 0;
    xLightsFrame* frame = static_cast<xLightsFrame*>(GetParent());
    wxJSONValue outputs = FPP::CreateOutputUniverseFile(_outputManager);
    std::string memoryMaps = FPP::CreateModelMemoryMap(&frame->AllModels);
    for (auto inst : instances) {
        std::string rowStr = std::to_string(row);
        if (!cancelled && doUpload[row]) {
            std::string playlist = GetChoiceValue(PLAYLIST_COL + rowStr);
            if (playlist != "") {
                cancelled |= inst->UploadPlaylist(playlist);
            }
            if (GetCheckValue(MODELS_COL + rowStr)) {
                cancelled |= inst->UploadModels(memoryMaps);
                inst->SetRestartFlag();
            }
            if (GetCheckValue(UDP_COL + rowStr)) {
                cancelled |= inst->UploadUDPOut(outputs);
                inst->SetRestartFlag();
            }
            if (GetCheckValue(UPLOAD_CONTROLLER_COL + rowStr)) {
                cancelled |= inst->UploadPixelOutputs(&frame->AllModels, _outputManager);
                inst->SetRestartFlag();
            }
        }
        row++;
    }
    if (!cancelled) {
        SaveSettings();
        EndDialog(wxID_CLOSE);
    }
}

void FPPConnectDialog::CreateDriveList()
{
    wxArrayString drives;
#ifdef __WXMSW__
    wxArrayString ud = wxFSVolume::GetVolumes(wxFS_VOL_REMOVABLE | wxFS_VOL_MOUNTED, 0);
    for (auto &a : ud) {
        if (wxDir::Exists(a + "\\sequences")) {
            drives.push_back(a);
        }
    }
#elif defined(__WXOSX__)
    wxDir d;
    d.Open("/Volumes");
    wxString dir;
    bool fcont = d.GetFirst(&dir, wxEmptyString, wxDIR_DIRS);
    while (fcont)
    {
        if (wxDir::Exists("/Volumes/" + dir + "/sequences")) { //raw USB drive mounted
            drives.push_back("/Volumes/" + dir + "/");
        } else if (wxDir::Exists("/Volumes/" + dir + "/media/sequences")) { // 1.x Mounted via SMB/NFS
            drives.push_back("/Volumes/" + dir + "/media/");
        }
        fcont = d.GetNext(&dir);
    }
#else
    bool done = false;
    wxDir d;
    d.Open("/media");
    wxString dir;
    bool fcont = d.GetFirst(&dir, wxEmptyString, wxDIR_DIRS);
    while (fcont) {
        wxDir d2;
        d2.Open("/media/" + dir);
        wxString dir2;
        bool fcont2 = d2.GetFirst(&dir2, wxEmptyString, wxDIR_DIRS);
        while (fcont2) {
            if (dir2 == "sequences") {
                drives.push_back("/media/" + dir + "/" + dir2);
            } else if (wxDir::Exists("/media/" + dir + "/" + dir2 + "/sequences")) {
                drives.push_back("/media/" + dir + "/" + dir2);
            }
            fcont2 = d2.GetNext(&dir2);
        }
        fcont = d.GetNext(&dir);
    }
#endif

    for (auto a : drives) {
        FPP *inst = new FPP();
        inst->hostName = "FPP";
        inst->ipAddress = a;
        inst->minorVersion = 0;
        inst->majorVersion = 2;
        inst->fullVersion = "Unknown";
        inst->mode = "standalone";
        if (wxFile::Exists(a + "/fpp-info.json")) {
            //read version and hostname
            wxJSONValue system;
            wxJSONReader reader;
            wxString str;
            wxFile(a + "/fpp-info.json").ReadAll(&str);
            reader.Parse(str, &system);

            if (!system["hostname"].IsNull()) {
                inst->hostName = system["hostname"].AsString();
            }
            if (!system["type"].IsNull()) {
                inst->platform = system["type"].AsString();
            }
            if (!system["model"].IsNull()) {
                inst->model = system["model"].AsString();
            }
            if (!system["version"].IsNull()) {
                inst->fullVersion = system["version"].AsString();
            }
            if (system["minorVersion"].IsInt()) {
                inst->minorVersion = system["minorVersion"].AsInt();
            }
            if (system["majorVersion"].IsInt()) {
                inst->majorVersion = system["majorVersion"].AsInt();
            }
            if (!system["channelRanges"].IsNull()) {
                inst->ranges = system["channelRanges"].AsString();
            }
            if (!system["HostDescription"].IsNull()) {
                inst->description = system["HostDescription"].AsString();
            }
            if (!system["fppModeString"].IsNull()) {
                inst->mode = system["fppModeString"].AsString();
            }
        }
        instances.push_back(inst);
    }
}

bool FPPConnectDialog::GetCheckValue(const std::string &col) {
    wxWindow *w = FPPInstanceList->FindWindow(col);
    if (w) {
        wxCheckBox *cb = dynamic_cast<wxCheckBox*>(w);
        if (cb) {
            return cb->GetValue();
        }
    }
    return false;
}
int FPPConnectDialog::GetChoiceValueIndex(const std::string &col) {
    wxWindow *w = FPPInstanceList->FindWindow(col);
    if (w) {
        wxItemContainer *cb = dynamic_cast<wxItemContainer*>(w);
        if (cb) {
            return cb->GetSelection();
        }
    }
    return 0;
}

std::string FPPConnectDialog::GetChoiceValue(const std::string &col) {
    wxWindow *w = FPPInstanceList->FindWindow(col);
    if (w) {
        wxItemContainer *cb = dynamic_cast<wxItemContainer*>(w);
        if (cb) {
            return cb->GetStringSelection();
        }
    }
    return "";
}
void FPPConnectDialog::SetChoiceValueIndex(const std::string &col, int i) {
    wxWindow *w = FPPInstanceList->FindWindow(col);
    if (w) {
        wxItemContainer *cb = dynamic_cast<wxItemContainer*>(w);
        if (cb) {
            return cb->SetSelection(i);
        }
    }
}
void FPPConnectDialog::SetCheckValue(const std::string &col, bool b) {
    wxWindow *w = FPPInstanceList->FindWindow(col);
    if (w) {
        wxCheckBox *cb = dynamic_cast<wxCheckBox*>(w);
        if (cb) {
            return cb->SetValue(b);
        }
    }

}

void FPPConnectDialog::SaveSettings()
{
    wxString selected = "";
    for (int fs = 0; fs < CheckListBox_Sequences->GetItemCount(); fs++) {
        if (CheckListBox_Sequences->IsItemChecked(fs)) {
            if (selected != "") {
                selected += ",";
            }
            selected += CheckListBox_Sequences->GetItemText(fs);
        }
    }
    wxConfigBase* config = wxConfigBase::Get();
    config->Write("FPPConnectSelectedSequences", selected);
    int row = 0;
    for (auto inst : instances) {
        std::string rowStr = std::to_string(row);
        wxString keyPostfx = inst->ipAddress;
        keyPostfx.Replace(":", "_");
        keyPostfx.Replace("/", "_");
        keyPostfx.Replace("\\", "_");
        keyPostfx.Replace(".", "_");
        config->Write("FPPConnectUpload_" + keyPostfx, GetCheckValue(CHECK_COL + rowStr));
        config->Write("FPPConnectUploadMedia_" + keyPostfx, GetCheckValue(MEDIA_COL + rowStr));
        config->Write("FPPConnectUploadFSEQType_" + keyPostfx, GetChoiceValueIndex(FSEQ_COL + rowStr));
        config->Write("FPPConnectUploadModels_" + keyPostfx, GetCheckValue(MODELS_COL + rowStr));
        config->Write("FPPConnectUploadUDPOut_" + keyPostfx, GetCheckValue(UDP_COL + rowStr));
        config->Write("FPPConnectUploadPixelOut_" + keyPostfx, GetCheckValue(UPLOAD_CONTROLLER_COL + rowStr));
        row++;
    }
    
    config->Flush();
}

void FPPConnectDialog::ApplySavedHostSettings()
{
    /*
     static const std::string CHECK_COL = "ID_UPLOAD_";
     static const std::string FSEQ_COL = "ID_FSEQTYPE_";
     static const std::string MEDIA_COL = "ID_MEDIA_";
     static const std::string MODELS_COL = "ID_MODELS_";
     static const std::string UDP_COL = "ID_UDPOUT_";
     static const std::string PLAYLIST_COL = "ID_PLAYLIST_";
     static const std::string UPLOAD_CONTROLLER_COL = "ID_CONTROLLER_";
     */

    
    wxConfigBase* config = wxConfigBase::Get();
    if (config != nullptr) {
        int row = 0;
        for (auto inst : instances) {
            std::string rowStr = std::to_string(row);
            wxString keyPostfx = inst->ipAddress;
            keyPostfx.Replace(":", "_");
            keyPostfx.Replace("/", "_");
            keyPostfx.Replace("\\", "_");
            keyPostfx.Replace(".", "_");
            
            bool bval;
            int lval;
            if (config->Read("FPPConnectUpload_" + keyPostfx, &bval)) {
                SetCheckValue(CHECK_COL + rowStr, bval);
            }
            if (config->Read("FPPConnectUploadFSEQType_" + keyPostfx, &lval)) {
                SetChoiceValueIndex(FSEQ_COL + rowStr, lval);
            }
            if (config->Read("FPPConnectUploadMedia_" + keyPostfx, &bval)) {
                SetCheckValue(MEDIA_COL + rowStr, bval);
            }
            if (config->Read("FPPConnectUploadModels_" + keyPostfx, &bval)) {
                SetCheckValue(MODELS_COL + rowStr, bval);
            }
            if (config->Read("FPPConnectUploadUDPOut_" + keyPostfx, &bval)) {
                SetCheckValue(UDP_COL + rowStr, bval);
            }
            if (config->Read("FPPConnectUploadPixelOut_" + keyPostfx, &bval)) {
                SetCheckValue(UPLOAD_CONTROLLER_COL + rowStr, bval);
            }
            row++;
        }
    }
}


void FPPConnectDialog::OnClose(wxCloseEvent& event)
{
    EndDialog(0);
}

void FPPConnectDialog::SequenceListPopup(wxListEvent& event)
{
    wxMenu mnu;
    mnu.Append(ID_MNU_SELECTALL, "Select All");
    mnu.Append(ID_MNU_SELECTNONE, "Clear Selections");
    mnu.Connect(wxEVT_MENU, (wxObjectEventFunction)&FPPConnectDialog::OnPopup, nullptr, this);
    PopupMenu(&mnu);
}

void FPPConnectDialog::OnAddFPPButtonClick(wxCommandEvent& event)
{
    wxTextEntryDialog dlg(this, "Find FPP Instance", "Enter IP address or hostname for FPP Instance");
    if (dlg.ShowModal() == wxID_OK && IsIPValidOrHostname(dlg.GetValue().ToStdString())) {
        std::string ipAd = dlg.GetValue().ToStdString();
        int curSize = instances.size();
        
        wxProgressDialog prgs("Gathering configuration for " + ipAd,
                              "Gathering configuration for " + ipAd, 100, this);
        prgs.Pulse("Gathering configuration for " + ipAd);
        prgs.Show();

        FPP inst(ipAd);
        if (inst.AuthenticateAndUpdateVersions()) {
            std::list<std::string> add;
            add.push_back(ipAd);
            FPP::Probe(add, instances);
            int cur = 0;
            for (auto &fpp : instances) {
                if (cur >= curSize) {
                    prgs.Pulse("Gathering configuration for " + fpp->hostName + " - " + fpp->ipAddress);
                    fpp->AuthenticateAndUpdateVersions();
                    fpp->probePixelControllerType();
                }
                cur++;
            }
            
            //it worked, we found some new instances, record this
            wxConfigBase* config = wxConfigBase::Get();
            wxString ip;
            config->Read("FPPConnectForcedIPs", &ip);
            if (!ip.Contains(dlg.GetValue())) {
                if (ip != "") {
                    ip += "|";
                }
                ip += dlg.GetValue();
                config->Write("FPPConnectForcedIPs", ip);
                config->Flush();
            }
            PopulateFPPInstanceList();
        }
        prgs.Hide();
    }
}
