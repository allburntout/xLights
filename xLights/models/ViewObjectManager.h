#ifndef VIEWOBJECTMANAGER_H
#define VIEWOBJECTMANAGER_H

#include <map>

#include "ObjectManager.h"

class ViewObject;
class xLightsFrame;

class ViewObjectManager : public ObjectManager
{
public:
    ViewObjectManager(xLightsFrame* xl);
    virtual ~ViewObjectManager();

    virtual BaseObject *GetObject(const std::string &name) const;
    ViewObject *GetViewObject(const std::string &name) const;

    ViewObject* CreateAndAddObject(const std::string &type);
    ViewObject* CreateObject(wxXmlNode *node) const;
    void AddViewObject(ViewObject *view_object);
    void Delete(const std::string &name);
    bool Rename(const std::string &oldName, const std::string &newName);

    void LoadViewObjects(wxXmlNode *objectNode);

    ViewObject *operator[](const std::string &name) const;
    std::map<std::string, ViewObject*>::const_iterator begin() const;
    std::map<std::string, ViewObject*>::const_iterator end() const;
    unsigned int size() const;

protected:
    ViewObject *createAndAddObject(wxXmlNode *node);

private:
    std::map<std::string, ViewObject *> view_objects;
    xLightsFrame* xlights;

    void clear();
};

#endif // VIEWOBJECTMANAGER_H
