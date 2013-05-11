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
BrowserOperation* CreateDefaultConstructor(BrowserClass* originClass);
void CreateLinkToPrivate(BrowserClass* originClass, BrowserClass* privateClass);
BrowserClass* CreatePrivateClass(BrowserClass* originClass);
BrowserArtifact* CreatePrivateClassArtifact(BrowserClass* cl);
BrowserArtifact* CreateDeploymentArtifact(BrowserClass* cl, BrowserDeploymentView* deploymentView);
BrowserNode* CreateDeploymentView(BrowserClass* cl);
bool RequestNewPrivateClass();
// need to have creation of private in origin and initialization of origin in private
void UpdateConstructors(BrowserClass* originClass, BrowserClass* privateClass);
bool FindPrivate(BrowserClass* originClass, BrowserClass*& cl, bool askForNewClass);
void SplitPrivate(BrowserClass* cl);
void UnsplitPrivate(BrowserClass* cl);
void MoveToPrivate(QList<BrowserNode*> nodes);
void MoveToPublic(BrowserNode* cl);


#endif // PRIVATECLASS_H
