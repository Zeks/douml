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
#include "browser/BrowserClassView.h"
#include <QMessageBox>

BrowserNode* CreateDefaultConstructor(BrowserClass* originClass)
{
    BrowserNode* constructor = originClass->addOperation();
    constructor->set_name(originClass->get_name());
    return constructor;
}

QList<BrowserNode*> GetConstructors(BrowserClass* originClass)
{
    QList<BrowserNode*> result;
    Q3ListViewItem* item;
    for(item = originClass->firstChild(); item!=0; item->nextSibling())
    {
        BrowserNode* asNode = static_cast<BrowserNode*>(item);
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
    BrowserNode* privateConstructor = CreateDefaultConstructor(privateClass);
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

    BrowserNode* qConstPointer = privateClass->addAttribute();
    AttributeData* qcData = static_cast<AttributeData*>(qConstPointer->get_data());
    qcData->set_isa_const_attribute(true);
    qcData->set_type(originClass->get_name());
    qConstPointer->set_name("qc_ptr");

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
        newConstructorInit = QString(asOperation->get_value("constructor-initializer")) + ", d_ptr(new " + privateClass->get_name() + "())";
    asOperation->set_value(nKeys, newConstructorInit);
    asOperation->modified();

}
void CreateLinkToPrivate(BrowserClass* originClass, BrowserClass* privateClass)
{

    QList<BrowserNode*> constructors = GetConstructors(originClass);
    // if there is no constructor - create one
    if(constructors.count() == 0)
        constructors.push_back(CreateDefaultConstructor(originClass));
    for(auto node : constructors)
    {
        InsertPrivateLinkIntoConstructorDefinition(node, privateClass);
    }

    BrowserNode* dPointer = originClass->addAttribute();
    AttributeData* dData = static_cast<AttributeData*>(dPointer->get_data());
    dData->set_type(originClass->get_name());
    dPointer->set_name("d_ptr");

    BrowserNode* dConstPointer = originClass->addAttribute();
    AttributeData* dcData = static_cast<AttributeData*>(dConstPointer->get_data());
    dcData->set_isa_const_attribute(true);
    dcData->set_type(originClass->get_name());
    dConstPointer->set_name("dc_ptr");
}

BrowserClass* CreatePrivateClass(BrowserClass* originClass)
{
    BrowserNode* parent = static_cast<BrowserNode*>(originClass->parent());
    BrowserClassView* asView = static_cast<BrowserClassView*>(parent);
    if(!asView)
        return nullptr;

    BrowserClass * privateClass = BrowserClass::add_class(FALSE, asView);
    privateClass->set_name(originClass->get_name() + "Private");
    privateClass->modified();

    CreateLinkToPublic(originClass, privateClass);
    CreateLinkToPrivate(originClass, privateClass);
    return privateClass;
}
}
BrowserArtifact* CreatePrivateClassArtifact(BrowserClass* cl)
{
    if(!cl)
        return nullptr;

    BrowserDeploymentView* deploymentView = nullptr;

    if(cl->get_associated_artifact() != nullptr)
        deploymentView = static_cast<BrowserDeploymentView*>(cl->get_associated_artifact()->parent());

    BrowserArtifact* artifact = CreateDeploymentArtifact(cl, deploymentView);
    if(!artifact)
        return artifact;

    artifact->get_data()->set_stereotype("source");
    Q3ValueList<BrowserClass *> associatedClasses;
    associatedClasses << cl;
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
        dn = BrowserArtifact::add_artifact(deploymentView);
    return dn;
}

BrowserNode* CreateDeploymentView(BrowserClass* cl)
{
    BrowserNode* packageNode = cl;
    while(packageNode->get_type() != UmlPackage && packageNode->parent())
        packageNode = static_cast<BrowserNode*>(packageNode->parent());

    BrowserDeploymentView * deploymentView = nullptr;
    if(packageNode)
    {
        deploymentView = BrowserDeploymentView::add_deployment_view(packageNode);
        deploymentView->set_name("classes");
    }
    return deploymentView;
}

bool RequestNewPrivateClass()
{
    if(QMessageBox::warning(0,QObject::tr("Attention!"), QObject::tr("Private class does not exist. Create private class?")) == QMessageBox::Ok)
        return true;
    return false;
}

bool FindPrivate(BrowserClass* originClass, BrowserClass*& privateClass, bool askForNewClass)
{
    //IdDict<BrowserClass> classes;
    privateClass = nullptr;
    QString className = originClass->get_name();
    QString privateName = className + "Private";
    IdIterator<BrowserClass> it(BrowserClass::all);
    while(it.current() != 0)
    {
        if(privateName == it.current()->get_name())
            break;
        ++it;
    }
    if(it.current() != 0)
        privateClass = static_cast<BrowserClass*>(it.current());
    if(privateClass == nullptr && askForNewClass && RequestNewPrivateClass())
        DirectPrivateSplit::CreatePrivateClass(originClass);
    if(privateClass == nullptr)
        return false;
    return true;
}

void MoveToPrivate(QList<BrowserNode*> nodes)
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

    for(auto node : nodes)
    {
        parent->takeItem(node);
        privateClass->insertItem(node);
    }
}


bool FindPublic(BrowserClass* originClass, BrowserClass*& publicClass)
{
    publicClass = nullptr;
    QString className = originClass->get_name();
    if(!className.contains("Private"))
    {
        QMessageBox::warning(0, QObject::tr("Warning!"), QObject::tr("This is not a private class!"));
        return false;
    }
    QString publicName = className.remove("Private");
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

void MoveToPublic(QList<BrowserNode*> nodes)
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
        QMessageBox::warning(0, QObject::tr("Warning!"), QObject::tr("No public class found"));
        return;
    }
    for(auto node : nodes)
    {
        parent->takeItem(node);
        publicClass->insertItem(node);
    }
}
