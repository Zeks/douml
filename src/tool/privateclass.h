#ifndef PRIVATECLASS_H
#define PRIVATECLASS_H

#include <QList>
#include <QString>
#include <functional>
class BrowserNode;
class BrowserClass;
class BrowserOperation;
class BrowserArtifact;
class BrowserArtifact;
class BrowserDeploymentView;
class BrowserPackage;
namespace DirectPrivateSplit
{
void CreateLinkToPublic(BrowserClass* originClass, BrowserClass* privateClass);
void CreateLinkToPrivate(BrowserClass* originClass, BrowserClass* privateClass);
void InsertPrivateLinkIntoConstructorDefinition(BrowserNode* constructor, BrowserNode* privateClass);
BrowserClass* CreatePrivateClass(BrowserClass* originClass);
}

bool VerifySingleParent(QList<BrowserNode*>);
BrowserClass* GetParent(QList<BrowserNode*> nodes);
QList<BrowserNode*> GetConstructors(BrowserClass* originClass);
BrowserNode* CreateDefaultConstructor(BrowserClass* originClass);
BrowserArtifact* CreatePrivateClassArtifact(BrowserClass* cl);
BrowserArtifact* CreateDeploymentArtifact(BrowserClass* cl, BrowserDeploymentView* deploymentView);
BrowserNode* CreateDeploymentView(BrowserClass* cl);
bool RequestNewPrivateClass();
bool FindPrivate(BrowserClass* originClass, BrowserClass*& privateClass, bool askForNewClass);
bool FindPublic(BrowserClass* originClass, BrowserClass*& publicClass);
void MoveToPrivate(QList<BrowserNode*> nodes);
void MoveToPublic(QList<BrowserNode *> nodes);


struct DoumlPluginBase
{
    QString pluginName;
    void SetName(QString);
    QString GetName();
};

struct DoumlPackagePlugin : public DoumlPluginBase
{
    std::function<void(BrowserPackage)> action;
};

struct DoumlClassPlugin : public DoumlPluginBase
{
    std::function<void(BrowserClass)> action;
};

struct DoumlNodesPlugin : public DoumlPluginBase
{
    std::function<void(QList<BrowserNode*>)> action;
};

struct PrivateClassMovePlugin : public DoumlNodesPlugin
{
    std::function<void(BrowserClass* , BrowserClass*)> createLinkToPublic;
    std::function<void(BrowserClass* , BrowserClass*)> createLinkToPrivate;
    std::function<void(BrowserNode* , BrowserNode*)> insertPrivateLinkIntoConstructorDefinition;
    std::function<BrowserClass*(BrowserClass*)> createPrivateClass;
};
struct PublicClassMovePlugin : public DoumlNodesPlugin
{
    std::function<void(BrowserClass* , BrowserClass*)> createLinkToPublic;
    std::function<void(BrowserClass* , BrowserClass*)> createLinkToPrivate;
    std::function<void(BrowserNode* , BrowserNode*)> insertPrivateLinkIntoConstructorDefinition;
    std::function<BrowserClass*(BrowserClass*)> createPrivateClass;
};

#endif // PRIVATECLASS_H
