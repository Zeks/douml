// *************************************************************************
//
// Copyright 2012-2013 Nikolai Marchenko.
//
// *************************************************************************
//
// Copyright 2004-2010 Bruno PAGES  .
// Copyright 2012-2013 Nikolai Marchenko.
// Copyright 2012-2013 Leonardo Guilherme.
//
// This file is part of the DOUML Uml Toolkit.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License Version 3.0 as published by
// the Free Software Foundation and appearing in the file LICENSE.GPL included in the
//  packaging of this file.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License Version 3.0 for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// e-mail : doumleditor@gmail.com
// home   : http://sourceforge.net/projects/douml
//
// *************************************************************************

#ifndef PRIVATECLASS_H
#define PRIVATECLASS_H

#include <QList>
#include <QString>
#include <functional>
class BrowserNode;
class BrowserClass;
class BrowserOperation;
class BrowserArtifact;
class BrowserDeploymentView;
class BrowserPackage;
namespace DirectPrivateSplit
{
void CreateLinkToPublic(BrowserClass* originClass, BrowserClass* privateClass);
void CreateLinkToPrivate(BrowserClass* originClass, BrowserClass* privateClass);
void InsertPrivateLinkIntoConstructorDefinition(BrowserNode* constructor, BrowserNode* privateClass);
}
namespace QtPrivateSplit
{
void CreateLinkToPublic(BrowserClass* originClass, BrowserClass* privateClass);
void CreateLinkToPrivate(BrowserClass* originClass, BrowserClass* privateClass);
void InsertPrivateLinkIntoConstructorDefinition(BrowserNode* constructor, BrowserNode* privateClass);
void SetupPrivateArtifact(QString className,BrowserArtifact*);
BrowserOperation* CreateQConstructor(BrowserClass* originClass, BrowserClass* privateClass);
void CreateQtConstructors(BrowserClass* originClass, BrowserClass* privateClass);
}
BrowserClass* CreatePrivateClass(BrowserClass* originClass);
BrowserClass* GetParent(QList<BrowserNode*> nodes);
QList<BrowserNode*> GetConstructors(BrowserClass* originClass);
BrowserNode* CreateDefaultConstructors(BrowserClass* originClass);

BrowserArtifact* CreateArtifact(BrowserClass* cl, QString deploymentName);
BrowserArtifact* CreateDeploymentArtifact(BrowserClass* cl, BrowserDeploymentView* deploymentView);
BrowserNode* CreateDeploymentView(BrowserClass* cl, QString deploymentName = QString(""));
BrowserDeploymentView* GetDeploymentViewForClass(BrowserClass* node, QString deploymentViewName = QString(""));
bool RequestNewPrivateClass();
bool FindPrivate(BrowserClass* originClass, BrowserClass*& privateClass, bool askForNewClass);
bool FindPublic(BrowserClass* originClass, BrowserClass*& publicClass);
void MoveToPrivate(QList<BrowserNode*> nodes);
void MoveToPublic(QList<BrowserNode *> nodes);
bool IsPrivateClass(BrowserNode *);


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
bool VerifySingleParent(QList<BrowserNode*>);
struct PrivateClassMovePlugin : public DoumlNodesPlugin
{
public:
    void Execute(QList<BrowserNode*> nodes);
private:
    void MoveToPrivate(QList<BrowserNode*> nodes);

    bool FindPrivate(BrowserClass* originClass, BrowserClass*& privateClass, bool askForNewClass);
    BrowserClass* CreatePrivateClass(BrowserClass* originClass);
    void AdjustConstructors(BrowserClass* originClass, BrowserClass* privateClass);
public:
    std::function<void(BrowserClass* , BrowserClass*)> createLinkToPublic;
    std::function<void(BrowserClass* , BrowserClass*)> createLinkToPrivate;
    std::function<void(BrowserNode* , BrowserNode*)> insertPrivateLinkIntoConstructorDefinition;
    std::function<void(QString className, BrowserArtifact*)> setupPrivateArtifact;
    std::function<void(BrowserClass*,BrowserClass* )> createPublicClassConstructors;


};
struct PublicClassMovePlugin : public DoumlNodesPlugin
{
public:
    void Execute(QList<BrowserNode*> nodes);
private:
    //bool VerifySingleParent(QList<BrowserNode*>);
    void MoveToPublic(QList<BrowserNode*> nodes);
    bool FindPublic(BrowserClass* originClass, BrowserClass*& publicClass);
//    std::function<void(BrowserClass* , BrowserClass*)> createLinkToPublic;
//    std::function<void(BrowserClass* , BrowserClass*)> createLinkToPrivate;
//    std::function<void(BrowserNode* , BrowserNode*)> insertPrivateLinkIntoConstructorDefinition;
//    std::function<BrowserClass*(BrowserClass*)> createPrivateClass;
//    std::function<void(BrowserClass*)> createPublicClassConstructors;
};

#endif // PRIVATECLASS_H
