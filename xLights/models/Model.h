#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <map>
#include <vector>
#include <list>

#include "ModelScreenLocation.h"
#include "../Color.h"
#include "BaseObject.h"
#include <wx/gdicmn.h>

class wxProgressDialog;
class DimmingCurve;
class wxXmlNode;
class ModelPreview;
class wxArrayString;
class wxPropertyGridInterface;
class wxPropertyGridEvent;
class ModelScreenLocation;
class ModelManager;
class xLightsFrame;
class OutputManager;
class wxPGProperty;

class NodeBaseClass;
typedef std::unique_ptr<NodeBaseClass> NodeBaseClassPtr;

namespace DrawGLUtils {
    class xlAccumulator;
    class xl3Accumulator;
}

enum {
    //GRIDCHANGE_REFRESH_DISPLAY = 0x0001,
    //GRIDCHANGE_MARK_DIRTY = 0x0002,
    //GRIDCHANGE_REBUILD_PROP_GRID = 0x0004,
    //GRIDCHANGE_REBUILD_MODEL_LIST = 0x0008,
    //GRIDCHANGE_UPDATE_ALL_MODEL_LISTS = 0x0010,
    GRIDCHANGE_SUPPRESS_HOLDSIZE = 0x00020,

    //GRIDCHANGE_MARK_DIRTY_AND_REFRESH = 0x0003
};

class Model : public BaseObject
{
    friend class LayoutPanel;
    friend class SubModel;

public:
    Model(const ModelManager &manager);
    virtual ~Model();
    static wxArrayString GetLayoutGroups(const ModelManager& mm);
    static std::string SafeModelName(const std::string& name)
    {
        wxString n(name.c_str());
        n.Replace(",", "", true);
        n.Replace("~", "", true);
        n.Replace("!", "", true);
        n.Replace(";", "", true);
        n.Replace("<", "", true);
        n.Replace(">", "", true);
        n.Replace("\"", "", true);
        n.Replace("\'", "", true);
        n.Replace("&", "", true);
        n.Replace(":", "", true);
        n.Replace("|", "", true);
        //n.Replace("#", "", true);
        n.Replace("@", "", true);
        n.Replace("/", "", true);
        n.Replace("\\", "", true);
        // Other characters I could remove
        // $%^*()?|][{}`.
        return n.ToStdString();
    }

    virtual std::string GetFullName() const { return name;}
    void Rename(std::string newName);
    int GetNumStrings() const { return parm1; }
    virtual int GetNumPhysicalStrings() const { return parm1; }

    std::string description;
    xlColor customColor;
    DimmingCurve *modelDimmingCurve;
    int _controller; // this is used to pass the selected controller name between property create and property change only

    virtual bool AllNodesAllocated() const { return true; }
    static void ParseFaceInfo(wxXmlNode *fiNode, std::map<std::string, std::map<std::string, std::string> > &faceInfo);
    static void WriteFaceInfo(wxXmlNode *fiNode, const std::map<std::string, std::map<std::string, std::string> > &faceInfo);
    wxString SerialiseFace() const;
    wxString SerialiseState()const ;
    std::map<std::string, std::map<std::string, std::string> > faceInfo;

    static void ParseStateInfo(wxXmlNode *fiNode, std::map<std::string, std::map<std::string, std::string> > &stateInfo);
    static void WriteStateInfo(wxXmlNode *fiNode, const std::map<std::string, std::map<std::string, std::string> > &stateInfo);
    std::map<std::string, std::map<std::string, std::string> > stateInfo;
    void AddFace(wxXmlNode* n);
    void AddState(wxXmlNode* n);
    void AddSubmodel(wxXmlNode* n);

    wxString SerialiseSubmodel() const;

    virtual const std::vector<std::string> &GetBufferStyles() const { return DEFAULT_BUFFER_STYLES; };
    virtual void GetBufferSize(const std::string &type, const std::string &camera, const std::string &transform, int &BufferWi, int &BufferHi) const;
    virtual void InitRenderBufferNodes(const std::string &type, const std::string &camera, const std::string &transform,
                                       std::vector<NodeBaseClassPtr> &Nodes, int &BufferWi, int &BufferHi) const;
    const ModelManager &GetModelManager() const {
        return modelManager;
    }
    virtual void AddASAPWork(uint32_t work, const std::string& from) override;
    virtual bool SupportsXlightsModel();
    static Model* GetXlightsModel(Model* model, std::string &last_model, xLightsFrame* xlights, bool &cancelled, bool download, wxProgressDialog* prog, int low, int high);
    virtual void ImportXlightsModel(std::string filename, xLightsFrame* xlights, float& min_x, float& max_x, float& min_y, float& max_y);
    virtual void ExportXlightsModel();
    void SetStartChannel(std::string startChannel);
    void ReloadModelXml() {
        SetFromXml(ModelXml, zeroBased);
    }

    static const std::vector<std::string> DEFAULT_BUFFER_STYLES;

    virtual bool StrandsZigZagOnString() const { return false;};
    int GetDefaultBufferWi() const {return BufferWi;}
    int GetDefaultBufferHt() const {return BufferHt;}

    void SetProperty(wxString property, wxString value, bool apply = false);
    virtual void AddProperties(wxPropertyGridInterface* grid, OutputManager* outputManager) override;
    virtual void UpdateProperties(wxPropertyGridInterface* grid, OutputManager* outputManager) override;
    virtual void AddControllerProperties(wxPropertyGridInterface *grid);
    virtual void UpdateControllerProperties(wxPropertyGridInterface* grid);
    virtual void DisableUnusedProperties(wxPropertyGridInterface *grid) {};
    virtual void AddTypeProperties(wxPropertyGridInterface* grid) override {};
    virtual void UpdateTypeProperties(wxPropertyGridInterface* grid) override {};
    virtual void AddSizeLocationProperties(wxPropertyGridInterface *grid) override;
    virtual void OnPropertyGridChanging(wxPropertyGridInterface *grid, wxPropertyGridEvent& event) {};
    virtual int OnPropertyGridSelection(wxPropertyGridInterface *grid, wxPropertyGridEvent& event) { return 0; };
    virtual void OnPropertyGridItemCollapsed(wxPropertyGridInterface *grid, wxPropertyGridEvent& event) {};
    virtual void OnPropertyGridItemExpanded(wxPropertyGridInterface *grid, wxPropertyGridEvent& event) {};
    /**
     * Returns a combination of:
     *     0x0001  -  Refresh displays
     *     0x0002  -  Mark settings as "dirty"/"changed"
     *     0x0004  -  Rebuild the property grid
     *     0x0008  -  Rebuild the model list
     *     0x0010  -  Update all model lists
     */
    virtual int OnPropertyGridChange(wxPropertyGridInterface *grid, wxPropertyGridEvent& event);
    virtual const ModelScreenLocation &GetModelScreenLocation() const = 0;
    virtual ModelScreenLocation &GetModelScreenLocation() = 0;

    bool IsNodeInBufferRange(size_t nodeNum, int x1, int y1, int x2, int y2);
protected:
    void AdjustStringProperties(wxPropertyGridInterface *grid, int newNum);
    std::string ComputeStringStartChannel(int x);
    void ApplyTransform(const std::string &transform,
                        std::vector<NodeBaseClassPtr> &Nodes,
                        int &bufferWi, int &bufferHi) const;
    void AdjustForTransform(const std::string &transform,
                            int &bufferWi, int &bufferHi) const;
    void ApplyTransparency(xlColor &color, int transparency) const;
    void DumpBuffer(std::vector<NodeBaseClassPtr> &newNodes, int bufferWi, int bufferHi) const;

    // size of the default buffer
    int BufferHt = 0;
    int BufferWi = 0;
    int BufferDp = 0;  
    std::vector<NodeBaseClassPtr> Nodes;
    const ModelManager &modelManager;

    virtual void InitModel();
    virtual int CalcCannelsPerString();
    virtual void SetStringStartChannels(bool zeroBased, int NumberOfStrings, int StartChannel, int ChannelsPerString);
    void RecalcStartChannels();

    void SetBufferSize(int NewHt, int NewWi);
    void SetNodeCount(size_t NumStrings, size_t NodesPerString, const std::string &rgbOrder);
    void CopyBufCoord2ScreenCoord();

    void SetLineCoord();
    std::string GetNextName();

    int pixelStyle;  //0 - default, 1 - smooth, 2 - circle
    int pixelSize = 2;
    int transparency = 0;
    int blackTransparency = 0;
    bool splitRGB;

    int StrobeRate;      // 0=no strobing
    bool zeroBased;

    std::vector<std::string> strandNames;
    std::vector<std::string> nodeNames;
    long parm1;         /* Number of strings in the model or number of arches or canes (except for frames & custom) */
    long parm2;         /* Number of nodes per string in the model or number of segments per arch or cane (except for frames & custom) */
    long parm3;         /* Number of strands per string in the model or number of lights per arch or cane segment (except for frames & custom) */
    bool IsLtoR;         // true=left to right, false=right to left
    std::vector<long> stringStartChan;
    bool isBotToTop;
    std::string StringType; // RGB Nodes, 3 Channel RGB, Single Color Red, Single Color Green, Single Color Blue, Single Color White
    int rgbwHandlingType;

    std::vector<Model *> subModels;
    void ParseSubModel(wxXmlNode *subModelNode);
    void ColourClashingChains(wxPGProperty* p);

    std::vector<std::string> modelState;

public:
    bool IsControllerConnectionValid() const;
    wxXmlNode *GetControllerConnection() const;
    std::string GetControllerConnectionString() const;
    std::string GetControllerConnectionRangeString() const;
    
    static std::list<std::string> GetProtocols();
    static std::list<std::string> GetLCProtocols();
    static bool IsProtocolValid(std::string protocol);
    static bool IsPixelProtocol(const std::string &protocol);

    bool IsPixelProtocol() const;
    long GetStringStartChan(int x) const {
        if (x < stringStartChan.size()) {
            return stringStartChan[x];
        }
        return 1;
    }
    void SetControllerName(const std::string& controllerName);
    void SetControllerProtocol(const std::string& protocol);
    void SetControllerPort(int port);
    std::string GetControllerName() const;
    std::string GetControllerProtocol() const;
    int GetSmartRemote() const;
    int GetControllerPort(int string = 1) const;
    void SetModelChain(const std::string& modelChain);
    std::string GetModelChain() const;
    const std::vector<Model *>& GetSubModels() const { return subModels; }
    Model *GetSubModel(const std::string &name);
    int GetNumSubModels() const { return subModels.size();}
    Model *GetSubModel(int i) const { return i < (int)subModels.size() ? subModels[i] : nullptr;}
    void RemoveSubModel(const std::string &name);
    std::list<int> ParseFaceNodes(std::string channels);

    unsigned long GetChangeCount() const { return changeCount; }

    std::string rgbOrder;
    bool SingleNode;     // true for dumb strings and single channel strings
    bool SingleChannel;  // true for traditional single-color strings

    std::string ModelStartChannel;
    bool CouldComputeStartChannel;
    bool Overlapping=false;
    std::string _pixelCount = "";
    std::string _pixelType = "";
    std::string _pixelSpacing = "";

    void SetFromXml(wxXmlNode* ModelNode, bool zeroBased=false) override;
    virtual bool ModelRenamed(const std::string &oldName, const std::string &newName);
    size_t GetNodeCount() const;
    int GetChanCount() const;
    size_t GetActChanCount() const;
    int GetChanCountPerNode() const;
    size_t GetCoordCount(size_t nodenum) const;
    int GetNodeStringNumber(size_t nodenum) const;
    void UpdateXmlWithScale() override;
    void SetPosition(double posx, double posy);
    virtual unsigned int GetLastChannel();
    std::string GetChannelInStartChannelFormat(OutputManager* outputManager, std::list<std::string>* visitedModels, unsigned int channel);
    std::string GetLastChannelInStartChannelFormat(OutputManager* outputManager);
    std::string GetFirstChannelInStartChannelFormat(OutputManager* outputManager);
    std::string GetStartChannelInDisplayFormat(OutputManager* outputManager);
    bool IsValidStartChannelString() const;
    virtual unsigned int GetFirstChannel() const;
    unsigned int GetNumChannels();
    int GetNodeNumber(size_t nodenum) const;
    int GetNodeNumber(int bufY, int bufX) const;
    bool UpdateStartChannelFromChannelString(std::map<std::string, Model*>& models, std::list<std::string>& used);
    int GetNumberFromChannelString(const std::string &sc) const;
    int GetNumberFromChannelString(const std::string &sc, bool &valid, std::string& dependsonmodel) const;
    virtual void DisplayModelOnWindow(ModelPreview* preview, DrawGLUtils::xlAccumulator &solidVa, DrawGLUtils::xlAccumulator &transparentVa, bool is_3d = false, const xlColor *color = NULL, bool allowSelected = false);
    virtual void DisplayModelOnWindow(ModelPreview* preview, DrawGLUtils::xl3Accumulator &solidVa3, DrawGLUtils::xl3Accumulator &transparentVa3, bool is_3d = false, const xlColor *color =  NULL, bool allowSelected = false);
    virtual void DisplayEffectOnWindow(ModelPreview* preview, double pointSize);
    virtual int NodeRenderOrder() {return 0;}
    wxString GetNodeNear(ModelPreview* preview, wxPoint pt);

    virtual bool CleanupFileLocations(xLightsFrame* frame) override;
    std::list<std::string> GetFaceFiles(const std::list<std::string>& facesUsed, bool all = false) const;
    void MoveHandle(ModelPreview* preview, int handle, bool ShiftKeyPressed, int mouseX, int mouseY);
    int GetSelectedHandle();
    int GetNumHandles();
    int GetSelectedSegment();
    bool HasCurve(int segment);
    void SetCurve(int segment, bool create);
    void AddHandle(ModelPreview* preview, int mouseX, int mouseY);
    virtual void InsertHandle(int after_handle, float zoom, int scale);
    virtual void DeleteHandle(int handle);

    std::vector<std::string> GetModelState() const;
    void SaveModelState( std::vector<std::string>& state );

    bool HitTest(ModelPreview* preview, glm::vec3& ray_origin, glm::vec3& ray_direction);
    bool IsContained(ModelPreview* preview, int x1, int y1, int x2, int y2);
    const std::string& GetStringType(void) const { return StringType; }
    virtual int NodesPerString() const;
    virtual int NodesPerString(int string) const { return NodesPerString(); }
    virtual int MapPhysicalStringToLogicalString(int string) const { return string; }
    virtual int GetLightsPerNode() const { return 1; } // default to one unless a model supports this
    wxCursor InitializeLocation(int &handle, wxCoord x,wxCoord y, ModelPreview* preview);

    int NodeStartChannel(size_t nodenum) const;
    const std::string &NodeType(size_t nodenum) const;
    virtual int MapToNodeIndex(int strand, int node) const;

    void GetNodeChannelValues(size_t nodenum, unsigned char *buf);
    void SetNodeChannelValues(size_t nodenum, const unsigned char *buf);
    xlColor GetNodeColor(size_t nodenum) const;
    virtual xlColor GetNodeMaskColor(size_t nodenum) const;
    void SetNodeColor(size_t nodenum, const xlColor &c);
    wxChar GetChannelColorLetter(wxByte chidx);
    std::string GetRGBOrder() const { return rgbOrder; }
    static char EncodeColour(const xlColor& c);
    char GetAbsoluteChannelColorLetter(long absoluteChannel); // absolute channel may or may not be in this model ... in which case a ' ' is returned

    virtual std::string ChannelLayoutHtml(OutputManager* outputManager);
    virtual void ExportAsCustomXModel() const;
    virtual std::string GetStartLocation() const;
    bool IsCustom(void);
    virtual bool SupportsExportAsCustom() const = 0;
    virtual bool SupportsWiringView() const = 0;
    size_t GetChannelCoords(wxArrayString& choices); //wxChoice* choices1, wxCheckListBox* choices2, wxListBox* choices3);
    static bool ParseFaceElement(const std::string& str, std::vector<wxPoint>& first_xy);
    static bool ParseStateElement(const std::string& str, std::vector<wxPoint>& first_xy);
    std::string GetNodeXY(const std::string& nodenumstr);
    std::string GetNodeXY(int nodeinx);

    void GetNodeCoords(int nodeidx, std::vector<wxPoint> &pts);

    bool GetIsLtoR() const {return IsLtoR;}
    bool GetIsBtoT() const {return isBotToTop;}
    virtual int GetStrandLength(int strand) const;

    float _savedWidth = 0;
    float _savedHeight = 0;
    float _savedDepth = 0;
    void SaveDisplayDimensions()
    {
        _savedWidth = GetWidth();
        _savedHeight = GetHeight();
        _savedDepth = GetDepth();
    }
    void RestoreDisplayDimensions();

    void GetMinScreenXY(float& minx, float& miny) const;
    virtual int GetNumStrands() const;
    std::string GetStrandName(size_t x, bool def = false) const {
        if (x < strandNames.size()) {
            return strandNames[x];
        }
        if (def) {
            return wxString::Format("Strand %ld", (long)x + 1).ToStdString();
        }
        return "";
    }
    virtual std::string GetNodeName(size_t x, bool def = false) const {
        if (x < nodeNames.size()) {
            return nodeNames[x];
        }
        if (def) {
            return wxString::Format("Node %ld", (long)x + 1).ToStdString();
        }
        return "";
    }

    static std::string StartChanAttrName(int idx)
    {
        return wxString::Format(wxT("String%i"),idx+1).ToStdString();  // a space between "String" and "%i" breaks the start channels listed in Indiv Start Chans
    }
    // returns true for models that only have 1 string and where parm1 does NOT represent the # of strings
    static bool HasOneString(const std::string& DispAs)
    {
        return (DispAs == "Window Frame" || DispAs == "Custom");
    }
    // true for dumb strings and traditional strings
    static bool HasSingleNode(const std::string& StrType)
    {
        static std::string Nodes(" Nodes");
        if (Nodes.size() > StrType.size()) return false;
        return StrType.find(Nodes) == std::string::npos;
    }
    // true for traditional strings
    static bool HasSingleChannel(const std::string& StrType)
    {
        return GetNodeChannelCount(StrType) == 1;
    }
    static int GetNodeChannelCount(const std::string & nodeType);

protected:
    int maxVertexCount;
};

template <class ScreenLocation>
class ModelWithScreenLocation : public Model {
public:
    virtual const ModelScreenLocation &GetModelScreenLocation() const {
        return screenLocation;
    }
    virtual ModelScreenLocation &GetModelScreenLocation() {
        return screenLocation;
    }
    virtual const ModelScreenLocation &GetBaseObjectScreenLocation() const {
        return screenLocation;
    }
    virtual ModelScreenLocation &GetBaseObjectScreenLocation() {
        return screenLocation;
    }
protected:
    ModelWithScreenLocation(const ModelManager &manager) : Model(manager) {}
    virtual ~ModelWithScreenLocation() {}
    ScreenLocation screenLocation;
};

#endif // MODEL_H
