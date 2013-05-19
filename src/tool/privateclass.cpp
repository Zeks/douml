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

#include "privateclass.h"
#include "browser/BrowserPackage.h"
#include "browser/BrowserClass.h"
#include "browser/BrowserArtifact.h"
#include "browser/BrowserOperation.h"
#include "browser/BrowserAttribute.h"
#include "browser/BrowserDeploymentNode.h"
#include "browser/BrowserDeploymentView.h"
#include "data/OperationData.h"
#include "data/AttributeData.h"
#include "data/ArtifactData.h"
#include "data/ClassData.h"
#include "data/ExtraMemberData.h"
#include "browser/BrowserClassView.h"
#include "browserfunctions/operationfuncs.h"
#include <QMessageBox>
#include <QSettings>




BrowserNode* CreateDefaultConstructors(BrowserClass* originClass, BrowserClass* privateClass)
{
    BrowserNode* constructor = originClass->addOperation();
    constructor->set_name(originClass->get_name());
    return constructor;
}
namespace QtPrivateSplit{
void CreateQtConstructors(BrowserClass* originClass, BrowserClass* privateClass)
{
    QList<BrowserNode*> constructors = GetConstructors(originClass);
    // if there is no constructor - create one
    if(constructors.count() == 0)
        constructors.push_back(CreateDefaultConstructors(originClass, privateClass));

    CreateQConstructor(originClass, static_cast<BrowserClass*>(privateClass));
}
}


QList<BrowserNode*> GetConstructors(BrowserClass* originClass)
{
    QList<BrowserNode*> result;
    Q3ListViewItem* item;
    for(item = originClass->firstChild(); item!=0; item = item->nextSibling())
    {

        BrowserNode* asNode = static_cast<BrowserNode*>(item);
        if(!asNode)
            continue;
        if(asNode->get_name() == originClass->get_name())
            result << asNode;
    }
    return result;
}

bool VerifySingleParent(QList<BrowserNode*> nodes)
{
    BrowserNode* parent = nullptr;
    for(auto node : nodes)
    {
        if(node->parent() != parent && parent != nullptr)
            return false;
        parent = static_cast<BrowserNode*>(node->parent());
    }
    return true;
}

BrowserClass* GetParent(QList<BrowserNode*> nodes)
{
    if(nodes.count() == 0)
        return nullptr;
    return static_cast<BrowserClass*>(nodes.at(0)->parent());
}

namespace DirectPrivateSplit
{
void CreateLinkToPublic(BrowserClass* originClass, BrowserClass* privateClass)
{
    BrowserNode* privateConstructor = CreateDefaultConstructors(originClass, privateClass);
    privateConstructor->set_name(privateClass->get_name());

    OperationData* privateConstructorData = static_cast<OperationData*>(privateConstructor->get_data());
    int originalParamCount = privateConstructorData->get_n_params();
    privateConstructorData->set_n_params(originalParamCount + 1);
    privateConstructorData->set_param_name(0, "_q_ptr");
    privateConstructorData->set_param_type(0, AType(originClass));
    privateConstructorData->set_param_dir(0, UmlInOut);
    int nKeys = privateClass->get_n_keys();
    privateClass->resize_n_keys(nKeys+1, true);
    privateClass->set_value(nKeys, ": q_ptr(_q_ptr), qc_ptr(const_cast<const " + originClass->get_name() + ">)(_q_ptr)");
    privateClass->set_key(nKeys, "constructor-initializer");

    // need to create a pointer to parent in private class
    BrowserNode* qPointer = privateClass->addAttribute();
    AttributeData* qData = static_cast<AttributeData*>(qPointer->get_data());
    qData->set_type(originClass->get_name());
    qPointer->set_name("q_ptr");
    qPointer->setText(0,qPointer->get_name());
    qPointer->modified();
    BrowserNode* qConstPointer = privateClass->addAttribute();
    AttributeData* qcData = static_cast<AttributeData*>(qConstPointer->get_data());
    qcData->set_isa_const_attribute(true);
    qcData->set_type(originClass->get_name());
    qConstPointer->set_name("qc_ptr");
    qConstPointer->setText(0,qConstPointer->get_name());
    qConstPointer->modified();
    privateClass->modified();
}
void InsertPrivateLinkIntoConstructorDefinition(BrowserNode* constructor, BrowserNode* privateClass)
{
    BrowserOperation* asOperation = static_cast<BrowserOperation*>(constructor);
    OperationData* constructorData = static_cast<OperationData*>(asOperation->get_data());
    // need to check if constructor already have initializer
    bool hasInitializer = QString(constructorData->get_cppdef()).contains(":");
    QString newConstructorInit;
    int nKeys = privateClass->get_n_keys();
    if(!hasInitializer)
    {

        asOperation->resize_n_keys(nKeys+1, true);
        asOperation->set_key(nKeys, "constructor-initializer");
        newConstructorInit = ": d_ptr(new "+ privateClass->get_name() + "())";
    }
    else
    {
        nKeys--;
        newConstructorInit = QString(asOperation->get_value("constructor-initializer")) + ", d_ptr(new " + privateClass->get_name() + "())";
    }
    asOperation->set_value(nKeys, newConstructorInit);
    asOperation->modified();

}
void CreateLinkToPrivate(BrowserClass* originClass, BrowserClass* privateClass)
{
    BrowserNode* dPointer = originClass->addAttribute();
    AttributeData* dData = static_cast<AttributeData*>(dPointer->get_data());
    dData->set_type(originClass->get_name());
    dPointer->set_name("d_ptr");
    dPointer->modified();
    dPointer->setText(0,dPointer->get_name());
    BrowserNode* dConstPointer = originClass->addAttribute();
    AttributeData* dcData = static_cast<AttributeData*>(dConstPointer->get_data());
    dcData->set_isa_const_attribute(true);
    dcData->set_type(originClass->get_name());
    dConstPointer->set_name("dc_ptr");
    dConstPointer->modified();
    dConstPointer->setText(0,dConstPointer->get_name());
}
}
BrowserClass* PrivateClassMovePlugin::CreatePrivateClass(BrowserClass* originClass)
{
    BrowserNode* parent = static_cast<BrowserNode*>(originClass->parent());
    BrowserClassView* asView = static_cast<BrowserClassView*>(parent);
    if(!asView)
        return nullptr;
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "DoUML", "settings");
    settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
    QString privatePart = settings.value("PIMPL/private_part").toString();

    BrowserClass * privateClass = BrowserClass::add_class(FALSE, asView, originClass->get_name() + privatePart);
    privateClass->set_name(originClass->get_name() + privatePart);
    privateClass->modified();

    if(createLinkToPublic)
        createLinkToPublic(originClass, privateClass);
    if(createLinkToPrivate)
        createLinkToPrivate(originClass, privateClass);

    if(createPublicClassConstructors)
        createPublicClassConstructors(originClass, privateClass);

    AdjustConstructors(originClass, privateClass);

    return privateClass;
}

BrowserDeploymentView* GetDeploymentViewForClass(BrowserClass *node, QString deploymentViewName)
{
    if(!node)
        return nullptr;

    if(node->get_associated_artifact())
        return static_cast<BrowserDeploymentView*>(node->get_associated_artifact()->parent());

    if(!node->parent() || !node->parent()->parent())
        return nullptr;
    //first, we try to find deployment view with the same name (if it is not forced)
    BrowserPackage* package = static_cast<BrowserPackage*>(node->parent()->parent());
    if(!package)
        return nullptr;
    QStringList names{node->parent()->text(0), deploymentViewName};
    auto deploymentNodes = GetDeploymentNodes(package, deploymentViewName);
    auto it = deploymentNodes.begin();
    for(QString& name : names)
    {
        it = std::find_if(deploymentNodes.begin(), deploymentNodes.end(), [&](BrowserNode* testedNode){ return testedNode->get_name() == name;});
        if(it != deploymentNodes.end())
            break;
    }
    if(it == deploymentNodes.end())
        return nullptr;
    return static_cast<BrowserDeploymentView*>(*it);
}

BrowserArtifact* CreateArtifact(BrowserClass* node, QString deploymentName)
{
    if(!node)
        return nullptr;
    if(node->get_associated_artifact() != nullptr)
        return node->get_associated_artifact();

    BrowserDeploymentView* deploymentView = nullptr;

    deploymentView = GetDeploymentViewForClass(node, deploymentName);

    if(!deploymentView)
        deploymentView = static_cast<BrowserDeploymentView*>(CreateDeploymentView(node, deploymentName));
    BrowserArtifact * artifact = nullptr;
    if(deploymentView)
    {
        auto artifacts = GetArtifactNodes(deploymentView, node->get_name());
        if(artifacts.size() > 0)
           artifact = static_cast<BrowserArtifact*>(artifacts.at(0));
        if(!artifact)
            artifact = BrowserArtifact::add_artifact(deploymentView, node->get_name());
    }

    artifact->get_data()->set_stereotype("source");
    Q3ValueList<BrowserClass *> associatedClasses;
    associatedClasses << node;
    artifact->set_associated_classes(associatedClasses);
    ArtifactData* artifactData = static_cast<ArtifactData*>(artifact->get_data());
    artifactData->use_default_cpp_h();
    artifactData->use_default_cpp_src();
    artifactData->use_default_idl_src();
    artifactData->use_default_java_src();
    artifactData->use_default_php_src();
    artifactData->use_default_python_src();
    return artifact;
}

BrowserArtifact* CreateDeploymentArtifact(BrowserClass* cl, BrowserDeploymentView* deploymentView)
{
    if(!deploymentView)
        deploymentView = static_cast<BrowserDeploymentView*>(CreateDeploymentView(cl));
    BrowserArtifact * dn = nullptr;
    if(deploymentView)
        dn = BrowserArtifact::add_artifact(deploymentView, cl->get_name());
    return dn;
}

BrowserNode* CreateDeploymentView(BrowserClass* cl, QString deploymentName)
{
    BrowserNode* packageNode = cl;
    while(packageNode->get_type() != UmlPackage && packageNode->parent())
        packageNode = static_cast<BrowserNode*>(packageNode->parent());

    BrowserDeploymentView * deploymentView = nullptr;
    if(packageNode)
    {
        deploymentView = BrowserDeploymentView::add_deployment_view(packageNode, deploymentName);
        deploymentView->set_name(deploymentName);
    }
    return deploymentView;
}

bool RequestNewPrivateClass()
{
    return true;
    if(QMessageBox::warning(0,QObject::tr("Attention!"), QObject::tr("Private class does not exist. Create private class?")) == QMessageBox::Ok)
        return true;
    return false;
}

bool PrivateClassMovePlugin::FindPrivate(BrowserClass* originClass, BrowserClass*& privateClass, bool askForNewClass)
{
    privateClass = nullptr;
    QString className = originClass->get_name();
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "DoUML", "settings");
    settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
    QString privatePart = settings.value("PIMPL/private_part").toString();
    QString privateName = className + privatePart;
    IdIterator<BrowserClass> it(BrowserClass::all);
    while(it.current() != 0)
    {
        if(privateName == it.current()->get_name() && !it.current()->deletedp())
            break;
        ++it;
    }
    if(it.current() != 0)
        privateClass = static_cast<BrowserClass*>(it.current());
    if(privateClass == nullptr && askForNewClass && RequestNewPrivateClass())
    {
        privateClass = CreatePrivateClass(originClass);
        BrowserArtifact* artifact = nullptr;
        artifact = CreateArtifact(privateClass, "PrivateClasses");
        if(setupPrivateArtifact)
           setupPrivateArtifact(privateClass->get_name(), artifact);
    }
    if(privateClass == nullptr)
        return false;
    return true;
}

void PrivateClassMovePlugin::MoveToPrivate(QList<BrowserNode*> nodes)
{
    if(!VerifySingleParent(nodes))
    {
        QMessageBox::warning(0, QObject::tr("Warning!"), QObject::tr("You can only move nodes if they have the same parent"));
        return;
    }
    BrowserClass* parent = GetParent(nodes);
    if(!parent)
        return;

    BrowserClass* privateClass = nullptr;
    if(!FindPrivate(parent, privateClass, true))
        return;
    privateClass->modified();
    for(auto node : nodes)
    {
        parent->takeItem(node);
        privateClass->insertItem(node);
        BrowserNode* insertSibling = static_cast<BrowserNode*>(privateClass->firstChild());
        for(int i = 0; i < privateClass->childCount()-1; i++)
        {
            insertSibling=static_cast<BrowserNode*>(insertSibling->nextSibling());
        }
        privateClass->move(node, insertSibling);
    }
}

bool FindPublic(BrowserClass* originClass, BrowserClass*& publicClass)
{
    publicClass = nullptr;
    QString className = originClass->get_name();
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "DoUML", "settings");
    settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
    QString privatePart = settings.value("PIMPL/private_part").toString();
    if(!className.contains(privatePart))
    {
        QMessageBox::warning(0, QObject::tr("Warning!"), QObject::tr("This is not a private class!"));
        return false;
    }
    QString publicName = className.remove(privatePart);
    IdIterator<BrowserClass> it(BrowserClass::all);
    while(it.current() != 0)
    {
        if(publicName == it.current()->get_name())
            break;
        ++it;
    }
    if(it.current() != 0)
        publicClass = static_cast<BrowserClass*>(it.current());
    if(publicClass == nullptr)
        return false;
    return true;
}

//void MoveToPublic(QList<BrowserNode*> nodes)
//{
//    if(!VerifySingleParent(nodes))
//    {
//        QMessageBox::warning(0, QObject::tr("Warning!"), QObject::tr("You can only move nodes if they have the same parent"));
//        return;
//    }
//    BrowserClass* parent = GetParent(nodes);
//    if(!parent)
//        return;

//    BrowserClass* publicClass = nullptr;
//    if(!FindPublic(parent, publicClass))
//    {
//        QMessageBox::warning(0, QObject::tr("Warning!"), QObject::tr("No public class found"));
//        return;
//    }
//    for(auto node : nodes)
//    {
//        parent->takeItem(node);
//        publicClass->insertItem(node);
//    }
//}

BrowserNode* ReachClass(BrowserNode * node)
{
    bool finished = false;
    while(node && !finished)
    {
        if(node->get_type() == UmlClass)
        {
            finished = true;
            continue;
        }
        if(node->parent())
        {
            node = static_cast<BrowserNode*>(node->parent());
            continue;
        }
        node = nullptr;
        finished = true;
    }
    return node;
}


bool IsPrivateClass(BrowserNode * node)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "DoUML", "settings");
    settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
    QString privatePart = settings.value("PIMPL/private_part").toString();
    BrowserNode* classNode = ReachClass(node);
    if(classNode)
        return classNode->get_name().right(privatePart.length()) == privatePart;
    return false;

}


void QtPrivateSplit::CreateLinkToPublic(BrowserClass *originClass, BrowserClass *privateClass)
{
    // need to create a pointer to parent in private class
    BrowserNode* originLinker = privateClass->add_extra_member();
    ExtraMemberData* originLinkerData = static_cast<ExtraMemberData*>(originLinker->get_data());
    originLinker->set_name("Q_DECLARE_PUBLIC");
    originLinkerData->set_cpp_decl("    Q_DECLARE_PUBLIC(" + originClass->get_name() + ")");
    originLinker->setText(0,"Q_DECLARE_PUBLIC");
    originLinker->modified();

    BrowserNode* qPointer = privateClass->addAttribute();
    AttributeData* qData = static_cast<AttributeData*>(qPointer->get_data());
    qData->set_type(originClass->get_name() + "*");
    qData->set_visibility(UmlPrivate);

    qPointer->set_name("q_ptr");
    qPointer->modified();
    qPointer->setText(0,qPointer->get_name());
//    BrowserNode* qObject = privateClass->add_extra_member();
//    ExtraMemberData* qData = static_cast<ExtraMemberData*>(qObject->get_data());
//    qData->set_cpp_decl("    Q_OBJECT\n");
//    qObject->set_name("Q_OBJECT");
//    qObject->setText(0,"Q_OBJECT");
    //qObject->modified();
}


void QtPrivateSplit::CreateLinkToPrivate(BrowserClass *originClass, BrowserClass *privateClass)
{
//    BrowserNode* qObject = originClass->add_extra_member();
//    ExtraMemberData* qData = static_cast<ExtraMemberData*>(qObject->get_data());
//    qData->set_cpp_decl("    Q_OBJECT\n");
//    qObject->setText(0,"Q_OBJECT");
//    qObject->modified();

    BrowserNode* privateLinker = originClass->add_extra_member();
    ExtraMemberData* privateLinkerData = static_cast<ExtraMemberData*>(privateLinker->get_data());
    privateLinkerData->set_cpp_decl("    Q_DECLARE_PRIVATE(" + privateClass->get_name() + ")");
    privateLinker->setText(0,"Q_DECLARE_PRIVATE");
    privateLinker->modified();

    BrowserNode* dPointer = originClass->addAttribute();
    AttributeData* dData = static_cast<AttributeData*>(dPointer->get_data());
    dData->set_type(privateClass->get_name() + "*");
    dData->set_visibility(UmlPrivate);
    dPointer->set_name("d_ptr");
    dPointer->modified();
    dPointer->setText(0,dPointer->get_name());
    BrowserNode* start = static_cast<BrowserNode*>(originClass->firstChild());
//    originClass->move(qObject, start);
//    originClass->move(start, qObject);
    originClass->move(privateLinker, start);
    originClass->move(start, privateLinker);
//    originClass->move(privateLinker, qObject);
}



namespace QtPrivateSplit
{
void InsertPrivateLinkIntoConstructorDefinition(BrowserNode *constructor, BrowserNode *privateClass)
{
    BrowserOperation* asOperation = static_cast<BrowserOperation*>(constructor);
    OperationData* constructorData = static_cast<OperationData*>(asOperation->get_data());
    bool hasInitializerAsKey = asOperation->has_key("constructor-initializer") != -1;
    bool hasInitializerInDef = QString(constructorData->get_cppdef()).contains("constructor-initializer");
    constructorData->new_body("q_ptr = this;",'c');

    if(!hasInitializerInDef)
    {
        QString cppDef = constructorData->get_cppdef();
        int pos = cppDef.find("${)}$");
        cppDef.insert(pos+4, "@{constructor-initializer}");
        constructorData->set_cppdef(cppDef);
    }
    // need to check if constructor already have initializer
    QString newConstructorInit;
    int nKeys = static_cast<BrowserNode*>(asOperation->parent())->get_n_keys();
    bool skip = constructorData->get_n_params() > 0 && QString(constructorData->get_param_name(0)) == "dd";
    if(!hasInitializerAsKey)
    {
        asOperation->resize_n_keys(nKeys+1, true);
        asOperation->set_key(nKeys, "constructor-initializer");
        newConstructorInit = ": d_ptr(new "+ privateClass->get_name() + "())";
    }
    else
    {
        if(!skip)
        {
            nKeys--;
            newConstructorInit = QString(asOperation->get_value("constructor-initializer")) + ", d_ptr(new " + privateClass->get_name() + "())";
        }
    }
    nKeys = static_cast<BrowserNode*>(asOperation->parent())->get_n_keys();
    if(!skip)
        asOperation->set_value(nKeys, newConstructorInit);
    asOperation->modified();

}


void SetupPrivateArtifact(QString className, BrowserArtifact * artifact)
{
    artifact->set_n_keys(1);
    artifact->set_key(0, "moc_value");
    artifact->set_value(0, "#include \"moc_" + className + ".cpp\"");
    ArtifactData* artifactData = static_cast<ArtifactData*>(artifact->get_data());
    QString src = artifactData->get_cpp_src();
    int pos = src.find("${namespace_end}");
    src = src.insert(pos, "@{moc_value}\n");
    artifactData->set_cpp_src(src);
}
}

BrowserOperation* QtPrivateSplit::CreateQConstructor(BrowserClass *originClass, BrowserClass *privateClass)
{
    BrowserNode* constructor = originClass->addOperation();
    constructor->set_name(originClass->get_name());
    BrowserOperation* asOperation = static_cast<BrowserOperation*>(constructor);
    OperationData* asOperationData = static_cast<OperationData*>(asOperation->get_data());
    asOperationData->set_uml_visibility(UmlPrivate);
    asOperation->setText(0, originClass->get_name());


    asOperationData->set_n_params(1);


    asOperationData->set_param_name(0, "dd");
    asOperationData->set_param_type(0, AType(privateClass));
    asOperationData->set_param_dir(0, UmlInOut);
    OperationFuncs::recompute_param(asOperation, 0, false);

//    asOperationData->set_n_params(2);

//    asOperationData->set_param_name(1, "parent");
//    AType t;
//    t.explicit_type = "QObject";
//    asOperationData->set_param_type(1, t);
//    asOperationData->set_param_dir(1, UmlInOut);
//    asOperationData->set_param_default_value(1,"QObject()");
//    OperationFuncs::recompute_param(asOperation, 1, false);

    int nKeys = privateClass->get_n_keys();


    asOperation->resize_n_keys(nKeys+1, true);
    asOperation->set_key(nKeys, "constructor-initializer");
    QString newConstructorInit;
    //newConstructorInit = ": d_ptr(&dd), QObject(parent)";
    newConstructorInit = ": d_ptr(&dd)";
    asOperation->set_value(nKeys, newConstructorInit);
    asOperation->modified();
    asOperationData->modified();

    BrowserNode* start = static_cast<BrowserNode*>(originClass->firstChild());
    originClass->move(constructor, start);
    originClass->move(start, constructor);

    return static_cast<BrowserOperation*>(constructor);
}


void PrivateClassMovePlugin::AdjustConstructors(BrowserClass *originClass, BrowserClass *privateClass)
{
    QList<BrowserNode*> constructors = GetConstructors(originClass);
    for(auto node : constructors)
    {
        if(insertPrivateLinkIntoConstructorDefinition)
            insertPrivateLinkIntoConstructorDefinition(node, privateClass);
    }
    BrowserNode* privateConstructor = CreateDefaultConstructors(privateClass, originClass);
    privateConstructor->set_name(privateClass->get_name());
}


void PrivateClassMovePlugin::Execute(QList<BrowserNode *> nodes)
{
    MoveToPrivate(nodes);
}


void PublicClassMovePlugin::Execute(QList<BrowserNode *> nodes)
{
    MoveToPublic(nodes);
}

void PublicClassMovePlugin::MoveToPublic(QList<BrowserNode *> nodes)
{
    if(!VerifySingleParent(nodes))
    {
        QMessageBox::warning(0, QObject::tr("Warning!"), QObject::tr("You can only move nodes if they have the same parent"));
        return;
    }
    BrowserClass* parent = GetParent(nodes);
    if(!parent)
        return;

    BrowserClass* publicClass = nullptr;
    if(!FindPublic(parent, publicClass))
    {
        QMessageBox::warning(0, QObject::tr("Warning!"), QObject::tr("No public class to move to!"));
        return;
    }
    publicClass->modified();
    for(auto node : nodes)
    {
        parent->takeItem(node);
        publicClass->insertItem(node);
        BrowserNode* insertSibling = static_cast<BrowserNode*>(publicClass->firstChild());
        for(int i = 0; i < publicClass->childCount()-1; i++)
        {
            insertSibling=static_cast<BrowserNode*>(insertSibling->nextSibling());
        }
        publicClass->move(node, insertSibling);
    }
}

bool PublicClassMovePlugin::FindPublic(BrowserClass *originClass, BrowserClass *&publicClass)
{
    publicClass = nullptr;
    QString className = originClass->get_name();
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "DoUML", "settings");
    settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
    QString privatePart = settings.value("PIMPL/private_part").toString();

    QString publicName = className.replace(privatePart, "");
    IdIterator<BrowserClass> it(BrowserClass::all);
    while(it.current() != 0)
    {
        if(publicName == it.current()->get_name())
            break;
        ++it;
    }
    if(it.current() != 0)
        publicClass = static_cast<BrowserClass*>(it.current());
    if(publicClass == nullptr)
        return false;
    return true;
}

