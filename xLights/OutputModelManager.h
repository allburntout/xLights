
#ifndef OUTPUT_MODEL_MANAGER_H
#define OUTPUT_MODEL_MANAGER_H

#include <list>

class xLightsFrame;
class Model;
class Output;

class OutputModelManager {
	
    Model* _modelToModelFromXml = nullptr;
	uint32_t _workASAP = 0;      // work to do asap
	uint32_t _setupTabWork = 0;  // work to do when we next switch to the setup tab
	uint32_t _layoutTabWork = 0; // work to do when we next switch to the layout tab
	xLightsFrame* _frame = nullptr;
    bool _workRequested = false;
    std::string _selectedModel = "";
#ifdef _DEBUG
    std::list<std::pair<uint32_t, std::string>> _sourceASAP;
    std::list < std::pair<uint32_t, std::string>> _sourceLayout;
    std::list < std::pair<uint32_t, std::string>> _sourceSetup;
    void Dump(const std::string& type, const std::list<std::pair<uint32_t, std::string>>& source);
    std::string DecodeWork(uint32_t work);
#endif	

public:

    static const uint32_t WORK_UPDATE_PROPERTYGRID              = 0x0001;
    static const uint32_t WORK_MODELS_REWORK_STARTCHANNELS      = 0x0002;
    static const uint32_t WORK_RELOAD_MODEL_FROM_XML            = 0x0004; // Note the model must remain valid until the message is processed
    static const uint32_t WORK_RELOAD_ALLMODELS                 = 0x0008;
    static const uint32_t WORK_RELOAD_OBJECTLIST                = 0x0010;
    static const uint32_t WORK_MODELS_CHANGE_REQUIRING_RERENDER = 0x0020;
    static const uint32_t WORK_RGBEFFECTS_CHANGE                = 0x0040;
    static const uint32_t WORK_RELOAD_PROPERTYGRID              = 0x0080;
    static const uint32_t WORK_NETWORK_CHANGE                   = 0x0100;
    static const uint32_t WORK_UPDATE_NETWORK_LIST              = 0x0200;
    static const uint32_t WORK_CALCULATE_START_CHANNELS         = 0x0400;
    static const uint32_t WORK_NETWORK_CHANNELSCHANGE           = 0x0800;
    static const uint32_t WORK_RESEND_CONTROLLER_CONFIG         = 0x1000;
    static const uint32_t WORK_SAVE_NETWORKS                    = 0x2000;
    static const uint32_t WORK_RELOAD_MODELLIST                 = 0x4000;
    static const uint32_t WORK_REDRAW_LAYOUTPREVIEW             = 0x8000;

	OutputModelManager() {}
	void SetFrame(xLightsFrame* frame)
	{
		_frame = frame;
	}
	uint32_t GetASAPWork() {
		auto res = _workASAP;
		_workASAP = 0;
        _workRequested = false;
#ifdef _DEBUG
        Dump("ASAP", _sourceASAP);
        _sourceASAP.clear();
#endif
		return res;
	}
	uint32_t GetSetupWork() {
		auto res = _setupTabWork;
		_setupTabWork = 0;
#ifdef _DEBUG
        Dump("Setup", _sourceSetup);
        _sourceSetup.clear();
#endif
        return res;
	}
	uint32_t GetLayoutWork() {
		auto res = _layoutTabWork;
		_layoutTabWork = 0;
#ifdef _DEBUG
        Dump("Layout", _sourceLayout);
        _sourceLayout.clear();
#endif
        return res;
	}	
    uint32_t ClearWork(const std::string& type, uint32_t currentwork, uint32_t work);
    void AddImmediateWork(uint32_t work, const std::string& from, Model* m = nullptr, Output* o = nullptr, const std::string& selectedModel = "");
    void AddASAPWork(uint32_t work, const std::string& from, Model* m = nullptr, Output* o = nullptr, const std::string& selectedModel = "");
    void AddSetupTabWork(uint32_t work, const std::string& from, Model* m = nullptr, Output* o = nullptr, const std::string& selectedModel = "");
    void AddLayoutTabWork(uint32_t work, const std::string& from, Model* m = nullptr, Output* o = nullptr, const std::string& selectedModel = "");
    Model* GetModelToReload();
    std::string GetSelectedModel();
    void ClearSelectedModel() { _selectedModel = ""; }
    void ForceSelectedModel(const std::string& name) { _selectedModel = name; }
    void ClearModelToReload() { _modelToModelFromXml = nullptr; _workASAP &= ~WORK_RELOAD_MODEL_FROM_XML; }
};

#endif 
