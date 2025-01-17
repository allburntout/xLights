
#include <wx/wx.h>
#include <wx/xml/xml.h>

#include "ModelGroup.h"
#include "ModelManager.h"
#include "SingleLineModel.h"
#include "ModelScreenLocation.h"
#include <log4cpp/Category.hh>

static const std::string HORIZ("Horizontal Stack");
static const std::string VERT("Vertical Stack");
static const std::string HORIZ_PER_MODEL("Horizontal Per Model");
static const std::string VERT_PER_MODEL("Vertical Per Model");
static const std::string SINGLELINE_AS_PIXEL("Single Line Model As A Pixel");
static const std::string DEFAULT_AS_PIXEL("Default Model As A Pixel");
static const std::string HORIZ_PER_MODELSTRAND("Horizontal Per Model/Strand");
static const std::string VERT_PER_MODELSTRAND("Vertical Per Model/Strand");
static const std::string OVERLAY_CENTER("Overlay - Centered");
static const std::string OVERLAY_SCALED("Overlay - Scaled");
static const std::string SINGLE_LINE("Single Line");
static const std::string PER_MODEL_DEFAULT("Per Model Default");
static const std::string PER_MODEL_PER_PREVIEW("Per Model Per Preview");
static const std::string PER_MODEL_SINGLE_LINE("Per Model Single Line");

std::vector<std::string> ModelGroup::GROUP_BUFFER_STYLES;

Model* ModelGroup::GetModel(std::string modelName)
{
    for (auto it = models.begin(); it != models.end(); ++it)
    {
        if ((*it)->GetFullName() == modelName)
        {
            return *it;
        }
    }

    return nullptr;
}

bool ModelGroup::ContainsModelGroup(ModelGroup* mg)
{
    std::list<Model*> visited;
    visited.push_back(this);

    bool found = false;
    for (auto it = models.begin(); !found && it != models.end(); ++it)
    {
        if ((*it) != nullptr && (*it)->GetDisplayAs() == "ModelGroup")
        {
            if (*it == mg)
            {
                found = true;
            }
            else
            {
                if (std::find(visited.begin(), visited.end(), *it) == visited.end())
                {
                    found |= dynamic_cast<ModelGroup*>(*it)->ContainsModelGroup(mg, visited);
                }
                else
                {
                    // already seen this group so dont follow
                }
            }
        }
    }

    return found;
}

bool ModelGroup::ContainsModelGroup(ModelGroup* mg, std::list<Model*>& visited)
{
    visited.push_back(this);

    bool found = false;
    for (auto it = models.begin(); !found && it != models.end(); ++it)
    {
        if ((*it) != nullptr && (*it)->GetDisplayAs() == "ModelGroup")
        {
            if (*it == mg)
            {
                found = true;
            }
            else
            {
                if (std::find(visited.begin(), visited.end(), *it) == visited.end())
                {
                    found |= dynamic_cast<ModelGroup*>(*it)->ContainsModelGroup(mg, visited);
                }
                else
                {
                    // already seen this group so dont follow
                }
            }
        }
    }

    return found;
}

bool ModelGroup::ContainsModel(Model* m)
{
    wxASSERT(m->GetDisplayAs() != "ModelGroup");

    std::list<Model*> visited;
    visited.push_back(this);

    bool found = false;
    for (auto it = models.begin(); !found && it != models.end(); ++it)
    {
        if ((*it)->GetDisplayAs() == "ModelGroup")
        {
            if (std::find(visited.begin(), visited.end(), *it) == visited.end())
            {
                found |= dynamic_cast<ModelGroup*>(*it)->ContainsModel(m, visited);
            }
            else
            {
                // already seen this group so dont follow
            }
        }
        else
        {
            if (m == *it)
            {
                found = true;
            }
        }
    }

    return found;
}

bool ModelGroup::ContainsModel(Model* m, std::list<Model*>& visited)
{
    visited.push_back(this);

    bool found = false;
    for (auto it = models.begin(); !found && it != models.end(); ++it)
    {
        if ((*it)->GetDisplayAs() == "ModelGroup")
        {
            if (std::find(visited.begin(), visited.end(), *it) == visited.end())
            {
                found |= dynamic_cast<ModelGroup*>(*it)->ContainsModel(m, visited);
            }
            else
            {
                // already seen this group so dont follow
            }
        }
        else
        {
            if (m == *it)
            {
                found = true;
            }
        }
    }

    return found;
}

const std::vector<std::string> &ModelGroup::GetBufferStyles() const {
    struct Initializer {
        Initializer() {
            GROUP_BUFFER_STYLES = Model::DEFAULT_BUFFER_STYLES;
            GROUP_BUFFER_STYLES.push_back(HORIZ);
            GROUP_BUFFER_STYLES.push_back(VERT);
            GROUP_BUFFER_STYLES.push_back(HORIZ_PER_MODEL);
            GROUP_BUFFER_STYLES.push_back(VERT_PER_MODEL);
            GROUP_BUFFER_STYLES.push_back(HORIZ_PER_MODELSTRAND);
            GROUP_BUFFER_STYLES.push_back(VERT_PER_MODELSTRAND);
            GROUP_BUFFER_STYLES.push_back(OVERLAY_CENTER);
            GROUP_BUFFER_STYLES.push_back(OVERLAY_SCALED);
            GROUP_BUFFER_STYLES.push_back(SINGLELINE_AS_PIXEL);
            GROUP_BUFFER_STYLES.push_back(DEFAULT_AS_PIXEL);

            GROUP_BUFFER_STYLES.push_back(PER_MODEL_DEFAULT);
            GROUP_BUFFER_STYLES.push_back(PER_MODEL_PER_PREVIEW);
            GROUP_BUFFER_STYLES.push_back(PER_MODEL_SINGLE_LINE);
        }
    };
    static Initializer ListInitializationGuard;
    return GROUP_BUFFER_STYLES;
}

ModelGroup::ModelGroup(wxXmlNode *node, const ModelManager &m, int w, int h) : ModelWithScreenLocation(m)
{
    ModelXml = node;
    screenLocation.previewW = w;
    screenLocation.previewH = h;
    Reset();
}

void LoadRenderBufferNodes(Model *m, const std::string &type, const std::string &camera, std::vector<NodeBaseClassPtr> &newNodes, int &bufferWi, int &bufferHi) {

    if (m == nullptr) return;

    if (m->GetDisplayAs() == "ModelGroup")
    {
        ModelGroup *g = dynamic_cast<ModelGroup*>(m);
        if (g != nullptr) {
            for (auto it = g->Models().begin(); it != g->Models().end(); ++it) {
                LoadRenderBufferNodes(*it, type, camera, newNodes, bufferWi, bufferHi);
            }
        }
        else {
            m->InitRenderBufferNodes(type, camera, "None", newNodes, bufferWi, bufferHi);
        }
    }
    else
    {
        m->InitRenderBufferNodes(type, camera, "None", newNodes, bufferWi, bufferHi);
    }
}

int ModelGroup::GetGridSize() const
{
    return wxAtoi(ModelXml->GetAttribute("GridSize", "400"));
}

bool ModelGroup::Reset(bool zeroBased) {
    this->zeroBased = zeroBased;
    selected = false;
    name = ModelXml->GetAttribute("name").ToStdString();

    DisplayAs = "ModelGroup";
    StringType = "RGB Nodes";

    layout_group = ModelXml->GetAttribute("LayoutGroup", "Unassigned");
    int gridSize = wxAtoi(ModelXml->GetAttribute("GridSize", "400"));
    std::string layout = ModelXml->GetAttribute("layout", "minimalGrid").ToStdString();
    defaultBufferStyle = layout;
    if (layout.compare(0, 9, "Per Model") == 0) {
        layout = "Default";
    }
    if (layout == "grid" || layout == "minimalGrid") {
        defaultBufferStyle = "Default";
    } else if (layout == "vertical") {
        defaultBufferStyle = VERT_PER_MODEL;
    } else if (layout == "horizontal") {
        defaultBufferStyle = HORIZ_PER_MODEL;
    }
    Nodes.clear();
    models.clear();
    modelNames.clear();
    changeCount = 0;
    wxArrayString mn = wxSplit(ModelXml->GetAttribute("models"), ',');
    int nc = 0;
    for (int x = 0; x < mn.size(); x++) {
        Model *c = modelManager.GetModel(mn[x].ToStdString());
        if (c != nullptr) {
            modelNames.push_back(c->GetFullName());
            models.push_back(c);
            changeCount += c->GetChangeCount();
            nc += c->GetNodeCount();
        }
        else if (mn[x] == "")
        {
            // silently ignore blank models
        }
        else
        {
            static log4cpp::Category &logger_base = log4cpp::Category::getInstance(std::string("log_base"));
            logger_base.warn("Model group '%s' contains model '%s' that does not exist.", (const char *)GetFullName().c_str(), (const char *)mn[x].c_str());
            return false;
        }
    }

    if (nc) {
        Nodes.reserve(nc);
    }

    for (Model *c : models) {
        int bw, bh;
        LoadRenderBufferNodes(c, "Per Preview No Offset", "2D", Nodes, bw, bh);
    }

    //now have all the nodes for all the models
    float minx = 99999;
    float maxx = -1;
    float miny = 99999;
    float maxy = -1;

    int minChan = 9999999;
    for (auto it = Nodes.begin(); it != Nodes.end(); ++it) {
        for (auto coord = (*it)->Coords.begin(); coord != (*it)->Coords.end(); ++coord) {
            minx = std::min(minx, coord->screenX);
            miny = std::min(miny, coord->screenY);
            maxx = std::max(maxx, coord->screenX);
            maxy = std::max(maxy, coord->screenY);
        }
        if ((*it)->ActChan < minChan) {
            minChan = (*it)->ActChan;
        }
    }
    if (miny < 0) {
        for (auto it = Nodes.begin(); it != Nodes.end(); ++it) {
            for (auto coord = (*it)->Coords.begin(); coord != (*it)->Coords.end(); ++coord) {
                coord->screenY -= miny;
            }
        }
        maxy -= miny;
        miny = 0;
    }
    if (minx < 0) {
        for (auto it = Nodes.begin(); it != Nodes.end(); ++it) {
            for (auto coord = (*it)->Coords.begin(); coord != (*it)->Coords.end(); ++coord) {
                coord->screenX -= minx;
            }
        }
        maxx -= minx;
        minx = 0;
    }

    bool minimal = layout != "grid";

    float midX = (minx + maxx) / 2.0;
    float midY = (miny + maxy) / 2.0;
    if (!minimal) {
        minx = 0;
        miny = 0;
        maxx = std::max(maxx, (float)screenLocation.previewW);
        maxy = std::max(maxy, (float)screenLocation.previewH);
    }
    double hscale = gridSize / maxy;
    double wscale = gridSize / maxx;
    if (maxy < gridSize && maxx < gridSize) {
        hscale = 1.0;
        wscale = 1.0;
    }
    if (minimal) {
        if ((maxy-miny+1) < gridSize && (maxx-minx+1) < gridSize) {
            hscale = 1.0;
            wscale = 1.0;
        } else {
            hscale = gridSize / (maxy - miny + 1);
            wscale = gridSize / (maxx - minx + 1);
        }
    }
    if (hscale > wscale) {
        hscale = wscale;
    } else {
        wscale = hscale;
    }
    maxx = -999999;
    maxy = -999999;
    float nminx = 999999;
    float nminy = 999999;
    BufferHt = 0;
    BufferWi = 0;

    for (auto it = Nodes.begin(); it != Nodes.end(); ++it) {
        for (auto coord = (*it)->Coords.begin(); coord != (*it)->Coords.end(); ++coord) {
            if (minimal) {
                coord->screenX = coord->screenX - minx;
                coord->screenY = coord->screenY - miny;
            }
            coord->bufX = coord->screenX * wscale;
            coord->bufY = coord->screenY * hscale;
            if (minimal) {
                coord->screenX = coord->screenX + minx - midX;
                coord->screenY = coord->screenY + miny - midY;
            } else {
                coord->screenX = coord->screenX - midX;
                coord->screenY = coord->screenY - midY;
            }
            maxx = std::max(maxx, (float)coord->screenX);
            maxy = std::max(maxy, (float)coord->screenY);
            nminx = std::min(nminx, (float)coord->screenX);
            nminy = std::min(nminy, (float)coord->screenY);
            BufferHt = std::max(BufferHt, (int)coord->bufY);
            BufferWi = std::max(BufferWi, (int)coord->bufX);
        }
        if (zeroBased) {
            (*it)->ActChan = (*it)->ActChan - minChan;
        }
    }

    BufferHt++;
    BufferWi++;
    if (!minimal) {
        BufferHt = std::max(BufferHt,(int)((float)screenLocation.previewH * hscale));
        BufferWi = std::max(BufferWi,(int)((float)screenLocation.previewW * wscale));
    }

    screenLocation.SetRenderSize(maxx - nminx + 1, maxy - nminy + 1);
    screenLocation.SetPosition(BufferWi / 2.0f, BufferHt / 2.0f);

    return true;
}

void ModelGroup::ResetModels()
{
    models.clear();
    wxArrayString mn = wxSplit(ModelXml->GetAttribute("models"), ',');
    for (int x = 0; x < mn.size(); x++) {
        Model *c = modelManager.GetModel(mn[x].ToStdString());
        if (c != nullptr) {
            models.push_back(c);
        }
    }
}

ModelGroup::~ModelGroup() {}

unsigned ModelGroup::GetFirstChannel() const
{
    unsigned first = 999999999;
    for (auto it = ModelNames().begin(); it != ModelNames().end(); ++it)
    {
        Model* mm = modelManager.GetModel(*it);
        if (mm != nullptr)
        {
            if (mm->GetFirstChannel() < first)
            {
                first = mm->GetFirstChannel();
            }
        }
    }

    if (first == 999999999)
    {
        first = 0;
    }

    return first;
}

unsigned ModelGroup::GetLastChannel()
{
    unsigned last = 0;
    for (auto it = ModelNames().begin(); it != ModelNames().end(); ++it)
    {
        Model* mm = modelManager.GetModel(*it);
        if (mm != nullptr)
        {
            if (mm->GetLastChannel() > last)
            {
                last = mm->GetLastChannel();
            }
        }
    }

    return last;
}

void ModelGroup::AddModel(const std::string &name) {
    wxString newVal = ModelXml->GetAttribute("models", "");
    if (newVal.size() > 0) {
        newVal += ",";
    }
    newVal += name;
    ModelXml->DeleteAttribute("models");
    ModelXml->AddAttribute("models", newVal);
    Reset();
}

void ModelGroup::ModelRemoved(const std::string &oldName) {
    bool changed = false;
    wxString newVal;
    for (int x = 0; x < modelNames.size(); x++) {
        if (modelNames[x] == oldName) {
            changed = true;
        } else {
            if (x != 0) {
                newVal += ",";
            }
            newVal += modelNames[x];
        }
    }
    if (changed) {
        ModelXml->DeleteAttribute("models");
        ModelXml->AddAttribute("models", newVal);
        Reset();
    }
}

bool ModelGroup::ModelRenamed(const std::string &oldName, const std::string &newName) {
    bool changed = false;
    wxString newVal;
    for (int x = 0; x < modelNames.size(); x++) {
        if (modelNames[x] == oldName) {
            modelNames[x] = newName;
            changed = true;
        }
        if (modelNames[x].find('/') != std::string::npos)
        {
            // this is a submodel
            std::string base = modelNames[x].substr(0, modelNames[x].find('/'));
            if (base == oldName)
            {
                auto startpos = modelNames[x].find('/');
                startpos++;
                modelNames[x] = newName + "/" + modelNames[x].substr(startpos);
                changed = true;
            }
        }
        if (x != 0) {
            newVal += ",";
        }
        newVal += modelNames[x];
    }
    if (changed) {
        ModelXml->DeleteAttribute("models");
        ModelXml->AddAttribute("models", newVal);
    }
    return changed;
}

bool ModelGroup::SubModelRenamed(const std::string &oldName, const std::string &newName) {
    bool changed = false;
    wxString newVal = "";
    for (int x = 0; x < modelNames.size(); x++) {
        if (modelNames[x] == oldName) {
            modelNames[x] = newName;
            changed = true;
        }
        if (x != 0) {
            newVal += ",";
        }
        newVal += modelNames[x];
    }
    if (changed) {
        ModelXml->DeleteAttribute("models");
        ModelXml->AddAttribute("models", newVal);
    }
    return changed;
}

void ModelGroup::CheckForChanges() const {

    unsigned long l = 0;
    for (auto it = models.begin(); it != models.end(); ++it) {
        l += (*it)->GetChangeCount();
    }

    if (l != changeCount) {
        // this is ugly ... it is casting away the const-ness of this
        ModelGroup *group = (ModelGroup*)this;
        if (group != nullptr) group->Reset();
    }
}

void ModelGroup::GetBufferSize(const std::string &tp, const std::string &camera, const std::string &transform, int &BufferWi, int &BufferHt) const {
    CheckForChanges();
    std::string type = tp;
    if (type.compare(0, 9, "Per Model") == 0) {
        type = "Default";
    }
    if (type == "Default") {
        type = defaultBufferStyle;
    }
    int models = 0;
    int strands = 0;
    int maxStrandLen = 0;
    int maxNodes = 0;
    int total = 0;

    int totWid = 0;
    int totHi = 0;
    int maxWid = 0;
    int maxHi = 0;
    for (auto it = modelNames.begin(); it != modelNames.end(); ++it) {
        Model* m = modelManager[*it];
        ModelGroup *grp = dynamic_cast<ModelGroup*>(m);
        if (grp != nullptr) {
            int bw, bh;
            bw = bh = 0;
            models++;
            grp->GetBufferSize(HORIZ_PER_MODELSTRAND, "2D", "None", bw, bh);
            strands += bw;
            total += m->GetNodeCount();
            if (m->GetNodeCount() > maxNodes) {
                maxNodes = m->GetNodeCount();
            }
            maxWid = std::max(maxWid, m->GetDefaultBufferWi());
            maxHi = std::max(maxHi, m->GetDefaultBufferHt());
            totWid += m->GetDefaultBufferWi();
            totHi += m->GetDefaultBufferHt();
            maxStrandLen = std::max(maxStrandLen, bh);
        } else if (m != nullptr) {
            models++;
            strands += m->GetNumStrands();
            total += m->GetNodeCount();
            if (m->GetNodeCount() > maxNodes) {
                maxNodes = m->GetNodeCount();
            }
            for (int x = 0; x < m->GetNumStrands(); x++) {
                if (m->GetStrandLength(x) > maxStrandLen) {
                    maxStrandLen = m->GetStrandLength(x);
                }
            }

            maxWid = std::max(maxWid, m->GetDefaultBufferWi());
            maxHi = std::max(maxHi, m->GetDefaultBufferHt());
            totWid += m->GetDefaultBufferWi();
            totHi += m->GetDefaultBufferHt();
        }
    }
    if (type == HORIZ_PER_MODEL) {
        BufferWi = models;
        BufferHt = maxNodes;
    } else if (type == VERT_PER_MODEL) {
        BufferHt = models;
        BufferWi = maxNodes;
    } else if (type == HORIZ) {
        BufferHt = maxHi;
        BufferWi = totWid;
    } else if (type == VERT) {
        BufferHt = totHi;
        BufferWi = maxWid;
    } else if (type == SINGLELINE_AS_PIXEL) {
        BufferHt = 1;
        BufferWi = models;
    } else if (type == DEFAULT_AS_PIXEL) {
        Model::GetBufferSize("Per Preview", camera, "None", BufferWi, BufferHt);
    } else if (type == HORIZ_PER_MODELSTRAND) {
        BufferWi = strands;
        BufferHt = maxStrandLen;
    } else if (type == VERT_PER_MODELSTRAND) {
        BufferHt = strands;
        BufferWi = maxStrandLen;
    } else if (type == SINGLE_LINE) {
        BufferHt = 1;
        BufferWi = total;
    } else if (type == OVERLAY_CENTER || type == OVERLAY_SCALED) {
        BufferHt = maxHi;
        BufferWi = maxWid;
    } else {
        Model::GetBufferSize(type, camera, "None", BufferWi, BufferHt);
    }
    AdjustForTransform(transform, BufferWi, BufferHt);

    wxASSERT(BufferWi != 0);
    wxASSERT(BufferHt != 0);
}

static inline void SetCoords(NodeBaseClass::CoordStruct &it2, int x, int y, int maxX, int maxY, int scale) {
    if (maxX != -1) {
        x = x * maxX;
        x = x / scale;
    }
    if (maxY != -1) {
        y = y * maxY;
        y = y / scale;
    }
    it2.bufX = x;
    it2.bufY = y;
}

void ModelGroup::InitRenderBufferNodes(const std::string &tp,
                                       const std::string& camera,
                                       const std::string &transform,
                                       std::vector<NodeBaseClassPtr> &Nodes,
                                       int &BufferWi, int &BufferHt) const {
    CheckForChanges();
    std::string type = tp;
    if (type.compare(0, 9, "Per Model") == 0) {
        type = "Default";
    }
    if (type == "Default") {
        type = defaultBufferStyle;
    }
    BufferWi = 0;
    BufferHt = 0;

    if (type == HORIZ_PER_MODEL) {
        for (auto it = modelNames.begin(); it != modelNames.end(); ++it) {
            Model* m = modelManager[*it];
            if (m != nullptr) {
                int start = Nodes.size();
                int x, y;
                m->InitRenderBufferNodes("Default", "2D", "None", Nodes, x, y);
                y = 0;
                while (start < Nodes.size()) {
                    for (auto it2 = Nodes[start]->Coords.begin(); it2 != Nodes[start]->Coords.end(); ++it2) {
                        it2->bufX = BufferWi;
                        it2->bufY = y;
                        y++;
                    }
                    start++;
                }
                if (y > BufferHt) {
                    BufferHt = y;
                }
                BufferWi++;
            }
        }
        ApplyTransform(transform, Nodes, BufferWi, BufferHt);
    } else if (type == HORIZ) {
        int modelX = 0;
        for (auto it = modelNames.begin(); it != modelNames.end(); ++it) {
            Model* m = modelManager[*it];
            if (m != nullptr) {
                int start = Nodes.size();
                int x, y;
                m->InitRenderBufferNodes("Default", "2D", "None", Nodes, x, y);
                while (start < Nodes.size()) {
                    for (auto it2 = Nodes[start]->Coords.begin(); it2 != Nodes[start]->Coords.end(); ++it2) {
                        it2->bufX = it2->bufX + modelX;
                    }
                    start++;
                }
                if (y > BufferHt) {
                    BufferHt = y;
                }
                BufferWi += x;
                modelX += x;
            }
        }
        ApplyTransform(transform, Nodes, BufferWi, BufferHt);
    } else if (type == VERT) {
        int modelY = 0;
        for (auto it = modelNames.begin(); it != modelNames.end(); ++it) {
            Model* m = modelManager[*it];
            if (m != nullptr) {
                int start = Nodes.size();
                int x, y;
                m->InitRenderBufferNodes("Default", "2D", "None", Nodes, x, y);
                while (start < Nodes.size()) {
                    for (auto it2 = Nodes[start]->Coords.begin(); it2 != Nodes[start]->Coords.end(); ++it2) {
                        it2->bufY = it2->bufY + modelY;
                    }
                    start++;
                }
                if (x > BufferWi) {
                    BufferWi = x;
                }
                BufferHt += y;
                modelY += y;
            }
        }
        ApplyTransform(transform, Nodes, BufferWi, BufferHt);
    } else if (type == VERT_PER_MODEL) {
        for (auto it = modelNames.begin(); it != modelNames.end(); ++it) {
            Model* m = modelManager[*it];
            if (m != nullptr) {
                int start = Nodes.size();
                int x, y;
                m->InitRenderBufferNodes("Default", "2D", "None", Nodes, x, y);
                y = 0;
                while (start < Nodes.size()) {
                    for (auto it2 = Nodes[start]->Coords.begin(); it2 != Nodes[start]->Coords.end(); ++it2) {
                        it2->bufY = BufferHt;
                        it2->bufX = y;
                        y++;
                    }
                    start++;
                }
                if (y > BufferWi) {
                    BufferWi = y;
                }
                BufferHt++;
            }
        }
        ApplyTransform(transform, Nodes, BufferWi, BufferHt);
    } else if (type == SINGLELINE_AS_PIXEL) {
        int outx = 0;
        BufferHt = 1;
        for (auto it = modelNames.begin(); it != modelNames.end(); ++it) {
            Model* m = modelManager[*it];
            if (m != nullptr) {
                int start = Nodes.size();
                int x = 0;
                int y = 0;
                m->InitRenderBufferNodes("As Pixel", "2D", "None", Nodes, x, y);
                while (start < Nodes.size()) {
                    for (auto it2 = Nodes[start]->Coords.begin(); it2 != Nodes[start]->Coords.end(); ++it2) {
                        it2->bufY = 0;
                        it2->bufX = outx;
                    }
                    start++;
                }
                outx++;
            }
        }
        BufferWi = outx;
        ApplyTransform(transform, Nodes, BufferWi, BufferHt);
    } else if (type == DEFAULT_AS_PIXEL) {
        int start = Nodes.size();
        Model::InitRenderBufferNodes("Per Preview", "2D", "None", Nodes, BufferWi, BufferHt);
        for (auto modelName : modelNames) {
            Model *c = modelManager.GetModel(modelName);
            if (c != nullptr) {
                int cx = 0;
                int cy = 0;
                int cnt = 0;
                //find the middle
                for (int x = 0; x < c->GetNodeCount(); x++) {
                    for (auto &coord : Nodes[start + x]->Coords) {
                        cx += coord.bufX;
                        cy += coord.bufY;
                        cnt++;
                    }
                }
                if (cnt != 0) {
                    cx /= cnt;
                    cy /= cnt;
                    for (int x = 0; x < c->GetNodeCount(); x++) {
                        for (auto &coord : Nodes[start + x]->Coords) {
                            coord.bufX = cx;
                            coord.bufY = cy;
                        }
                    }
                }
                start += c->GetNodeCount();
            }
        }
        ApplyTransform(transform, Nodes, BufferWi, BufferHt);
    } else if (type == HORIZ_PER_MODELSTRAND || type == VERT_PER_MODELSTRAND) {
        GetBufferSize(type, "2D", "None", BufferWi, BufferHt);
        bool horiz = type == HORIZ_PER_MODELSTRAND;
        int curS = 0;
        double maxSL = BufferWi;
        if (horiz) {
            maxSL = BufferHt;
        }
        for (auto it = modelNames.begin(); it != modelNames.end(); ++it) {
            Model* m = modelManager[*it];
            ModelGroup *grp = dynamic_cast<ModelGroup*>(m);
            int startBM = Nodes.size();
            if (grp != nullptr) {
                int bw, bh;
                bw = bh = 0;
                grp->InitRenderBufferNodes(type, "2D", "None", Nodes, bw, bh);
                for (int x = startBM; x < Nodes.size(); x++) {
                    for (auto it2 = Nodes[x]->Coords.begin(); it2 != Nodes[x]->Coords.end(); ++it2) {
                        if (horiz) {
                            it2->bufX += curS;
                        } else {
                            it2->bufY += curS;
                        }
                    }
                }
                if (horiz) {
                    curS += bw;
                } else {
                    curS += bh;
                }
            } else if (m != nullptr) {
                int bw, bh;
                bw = bh = 0;
                m->InitRenderBufferNodes(horiz ? "Horizontal Per Strand" : "Vertical Per Strand", "2D", "None", Nodes, bw, bh);
                for (int x = startBM; x < Nodes.size(); x++) {
                    for (auto it2 = Nodes[x]->Coords.begin(); it2 != Nodes[x]->Coords.end(); ++it2) {
                        if (horiz) {
                            SetCoords(*it2, it2->bufX + curS, it2->bufY, -1, BufferHt, bh);
                        } else {
                            SetCoords(*it2, it2->bufX, it2->bufY + curS, BufferWi, -1, bw);
                        }
                    }
                }
                if (horiz) {
                    curS += bw;
                } else {
                    curS += bh;
                }
            }
            if (m != nullptr) {
                int endBM = Nodes.size();
                if ((endBM - startBM) != m->GetNodeCount()) {
                    static log4cpp::Category &logger_base = log4cpp::Category::getInstance(std::string("log_base"));
                    logger_base.warn("Model group '%s' had problems creating render buffer for Per Strand/Model. Problem model '%s'.",
                                     (const char *)GetFullName().c_str(),
                                     (const char *)m->GetFullName().c_str());
                }
            }
        }
        ApplyTransform(transform, Nodes, BufferWi, BufferHt);
    } else if (type == SINGLE_LINE) {
        BufferHt = 1;
        BufferWi = 0;
        for (auto it = modelNames.begin(); it != modelNames.end(); ++it) {
            Model* m = modelManager[*it];
            if (m != nullptr) {
                int start = Nodes.size();
                int x, y;
                m->InitRenderBufferNodes("Single Line", "2D", "None", Nodes, x, y);
                while (start < Nodes.size()) {
                    for (auto it2 = Nodes[start]->Coords.begin(); it2 != Nodes[start]->Coords.end(); ++it2) {
                        it2->bufX = BufferWi;
                        it2->bufY = 0;
                    }
                    start++;
                    BufferWi++;
                }
            }
        }

        // Buffer widths of zero cause crashes elsewhere in effects so force it to be at least 1 wide
        if (BufferWi < 1) BufferWi = 1;

        ApplyTransform(transform, Nodes, BufferWi, BufferHt);
    } else if (type == OVERLAY_CENTER || type == OVERLAY_SCALED) {
        bool scale = type == OVERLAY_SCALED;
        GetBufferSize(type, "2D", "None", BufferWi, BufferHt);
        for (auto it = modelNames.begin(); it != modelNames.end(); ++it) {
            Model* m = modelManager[*it];
            if (m != nullptr) {
                int start = Nodes.size();
                int bw, bh;
                m->InitRenderBufferNodes("Default", "2D", "None", Nodes, bw, bh);
                if (bw != BufferWi || bh != BufferHt) {
                    //need to either scale or center
                    int offx = (BufferWi - bw)/2;
                    int offy = (BufferHt - bh)/2;
                    while (start < Nodes.size()) {
                        for (auto it2 = Nodes[start]->Coords.begin(); it2 != Nodes[start]->Coords.end(); ++it2) {
                            if (scale) {
                                it2->bufX = it2->bufX * (BufferWi/bw);
                                it2->bufY = it2->bufY * (BufferHt/bh);
                            } else {
                                it2->bufX += offx;
                                it2->bufY += offy;
                            }
                        }
                        start++;
                    }
                }
            }
        }
    } else {
        if( camera == "2D" && type == "Per Preview" ) {
            Nodes.clear();
        }
        Model::InitRenderBufferNodes(type, camera, transform, Nodes, BufferWi, BufferHt);
    }

    //wxASSERT(BufferWi != 0);
    //wxASSERT(BufferHt != 0);
}
