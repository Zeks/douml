#ifndef PRIVATECLASS_H
#define PRIVATECLASS_H

#include <QList>
class BrowserNode;
class BrowserClass;
class BrowserOperation;
class BrowserArtifact;
class BrowserDeploymentView;

bool VerifySingleParent(QList<BrowserNode*>);
BrowserClass* GetParent(QList<BrowserNode*> nodes);
void CreateLinkToPublic(BrowserClass* originClass, BrowserClass* privateClass);
QList<BrowserNode*> GetConstructors(BrowserClass* originClass);
void InsertPrivateLinkIntoConstructorDefinition(BrowserNode* constructor, BrowserNode* privateClass);
BrowserNode* CreateDefaultConstructor(BrowserClass* originClass);
void CreateLinkToPrivate(BrowserClass* originClass, BrowserClass* privateClass);
BrowserClass* CreatePrivateClass(BrowserClass* originClass);
BrowserArtifact* CreatePrivateClassArtifact(BrowserClass* cl);
BrowserArtifact* CreateDeploymentArtifact(BrowserClass* cl, BrowserDeploymentView* deploymentView);
BrowserNode* CreateDeploymentView(BrowserClass* cl);
bool RequestNewPrivateClass();
void UpdateConstructors(BrowserClass* originClass, BrowserClass* privateClass);
bool FindPrivate(BrowserClass* originClass, BrowserClass*& privateClass, bool askForNewClass);
bool FindPublic(BrowserClass* originClass, BrowserClass*& publicClass);
void MoveToPrivate(QList<BrowserNode*> nodes);
void MoveToPublic(QList<BrowserNode *> nodes);


#endif // PRIVATECLASS_H
