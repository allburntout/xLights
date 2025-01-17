#ifndef IMPORTPREVIEWSMODELSDIALOG_H
#define IMPORTPREVIEWSMODELSDIALOG_H

//(*Headers(ImportPreviewsModelsDialog)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
//*)

#include <wx/treelist.h>
#include <wx/xml/xml.h>

#include "models/ModelManager.h"

class LayoutGroup;

class ImportPreviewsModelsDialog: public wxDialog
{
    wxTreeListCtrl* TreeListCtrl1;
    wxXmlDocument _doc;
    wxTreeListItem _item;
    ModelManager& _allModels;
    std::vector<LayoutGroup*>& _layoutGroups;

    void ValidateWindow();
    void AddModels(wxTreeListCtrl* tree, wxTreeListItem item, wxXmlNode* models, wxString preview);
    void SelectAll(bool checked);
    void SelectSiblings(wxTreeListItem item, bool checked);
    void ExpandAll(bool expand);
    void DeselectExistingModels();
    bool ModelExists(const std::string& modelName) const;
    bool LayoutExists(const std::string& layoutName) const;

	public:

		ImportPreviewsModelsDialog(wxWindow* parent, const wxString& filename, ModelManager& allModels, std::vector<LayoutGroup*>& layoutGroups, wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~ImportPreviewsModelsDialog();
        wxArrayString GetPreviews() const;
        std::list<std::pair<wxString, wxXmlNode*>> GetModelsInPreview(wxString preview) const;
		//(*Declarations(ImportPreviewsModelsDialog)
		wxButton* Button_Cancel;
		wxButton* Button_Ok;
		wxFlexGridSizer* FlexGridSizer2;
		//*)

	protected:

		//(*Identifiers(ImportPreviewsModelsDialog)
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		//*)

        static const long ID_MNU_IPM_EXPANDALL;
        static const long ID_MNU_IPM_COLLAPSEALL;
        static const long ID_MNU_IPM_SELECTALL;
        static const long ID_MNU_IPM_DESELECTALL;
        static const long ID_MNU_IPM_SELECTSIBLINGS;
        static const long ID_MNU_IPM_DESELECTSIBLINGS;
        static const long ID_MNU_IPM_DESELECTEXISTING;

	private:

		//(*Handlers(ImportPreviewsModelsDialog)
		void OnButton_CancelClick(wxCommandEvent& event);
		void OnButton_OkClick(wxCommandEvent& event);
		//*)

        void OnContextMenu(wxTreeListEvent& event);
        void OnListPopup(wxCommandEvent& event);
        void OnTreeListCtrlCheckboxtoggled(wxTreeListEvent& event);

		DECLARE_EVENT_TABLE()
};

#endif
