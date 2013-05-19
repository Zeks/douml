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

#include "ui/menudispatcher.h"
#include "browser/BrowserClass.h"
#include "browser/BrowserAttribute.h"
#include "browser/BrowserOperation.h"
#include "ProfiledStereotypes.h"
#include "tool/Tool.h"
#include "menufactory.h"
#include "data/AttributeData.h"
#include "data/OperationData.h"
#include "translate.h"
#include "slots/nodeslots.h"
#include "slots/attributeslots.h"
#include "slots/operationslots.h"
#include "tool/privateclass.h"
#include "GenerationSettings.h"
#include "BrowserActivity.h"
#include "BrowserState.h"

MenuDispatcher::MenuDispatcher()
{
    AddMenu(TypeIdentifier<BrowserAttribute>::id(), AttributeMenu);
    AddMenu(TypeIdentifier<BrowserOperation>::id(), OperationMenu);
}

MenuDispatcher::~MenuDispatcher()
{
    DeleteLastMenu();
}

QMenu *MenuDispatcher::GetMenu(uint classID, BrowserNode * node)
{
    DeleteLastMenu();
    QMenu* newMenu = nullptr;
    if(menuCreators.contains(classID))
        newMenu = menuCreators[classID](node, lastMenus);
    return newMenu;
}

void MenuDispatcher::AddMenu(uint classID, std::function<QMenu *(BrowserNode *, QList<QMenu*>&)> menuCreator)
{
    menuCreators.insert(classID, menuCreator);
}


void MenuDispatcher::DeleteLastMenu()
{
    for(auto& menu : lastMenus)
    {
        if(menu)
        {
            delete menu;
            menu = nullptr;
        }
    }
    lastMenus.clear();
}

QMenu* AttributeMenu(BrowserNode* node, QList<QMenu*>& menuList)
{
    BrowserAttribute* attr = static_cast<BrowserAttribute*>(node);
    QMenu* nodeMenu = new QMenu();
    menuList << nodeMenu;
    QString stereotype = QString(((BrowserClass *) attr->parent())->get_stereotype());
    bool item = ((stereotype == "enum_pattern" || stereotype == "enum") && QString(attr->get_stereotype()) == "attribute");
    MenuFactory::createTitle(*nodeMenu, attr->def->definition(FALSE, TRUE));
    nodeMenu->insertSeparator();

    if (!attr->deletedp())
    {
        if (!attr->is_edited && (attr->get_container(UmlClass) != 0))
            nodeMenu->addAction(TR("Up"), attr->nodeSlots.get(), SLOT(OnGoUpOneLevel()))->setParent(nodeMenu);
        //QString editEntity = item ? TR("item") : TR("attribute");
        nodeMenu->addAction(TR("Edit"), attr->nodeSlots.get(), SLOT(OnEdit()))->setParent(nodeMenu);

        bool editable = attr->is_read_only && (attr->edition_number == 0);
        bool requiresGetOperation = !item && (attr->get_oper == 0) && editable;
        bool requiresSetOperation = !item && (attr->set_oper == 0) && editable;
        bool requiresBothGetAndSetOperations = requiresGetOperation && requiresSetOperation;
        if(requiresGetOperation)
            nodeMenu->addAction(TR("New get operation"), attr->nodeSlots.get(), SLOT(OnAddGetOperation()))->setParent(nodeMenu);
        if(requiresSetOperation)
            nodeMenu->addAction(TR("New set operation"), attr->nodeSlots.get(), SLOT(OnAddSetOperation()))->setParent(nodeMenu);
        if(requiresBothGetAndSetOperations)
            nodeMenu->addAction(TR("New get and set operation"), attr->nodeSlots.get(), SLOT(OnAddGetAndSetOperations()))->setParent(nodeMenu);
        if(GenerationSettings::cpp_get_default_defs())
        {
            if(IsPrivateClass(node))
                nodeMenu->addAction(TR("Move to public class"), attr->nodeSlots.get(), SLOT(OnMoveToPublicClass()))->setParent(nodeMenu);
            else
                nodeMenu->addAction(TR("Move to private class"), attr->nodeSlots.get(), SLOT(OnMoveToPrivateClass()))->setParent(nodeMenu);
        }
        if(editable)
            nodeMenu->addAction(TR("Duplicate"), attr->nodeSlots.get(), SLOT(OnDuplicate()))->setParent(nodeMenu);
        nodeMenu->addAction(TR("Referenced by"), attr->nodeSlots.get(), SLOT(OnShowReferencedBy()))->setParent(nodeMenu);
        if(editable)
        {
            nodeMenu->insertSeparator();
            nodeMenu->addAction(TR("Delete"), attr->nodeSlots.get(), SLOT(OnDeleteItem()))->setParent(nodeMenu);
        }
        nodeMenu->insertSeparator();
        attr->markMenu(nodeMenu);
        An<ProfiledStereotypes>()->Menu(nodeMenu, attr);
        QMenu * toolMenu = new QMenu();
        if(attr->edition_number == 0)
        {
            Tool::Menu(toolMenu, attr, attr->get_type(), attr->GetToolMenuBase());

            nodeMenu->addSeparator();
            toolMenu->setTitle(TR("Tools"));
            nodeMenu->addMenu(toolMenu);
            nodeMenu->addSeparator();
            //menuList << toolMenu;
        }
     }
    else
        nodeMenu->addAction(TR("Undelete"), attr->nodeSlots.get(), SLOT(OnUndeleteItem()))->setParent(nodeMenu);
    return nodeMenu;
}


QMenu* OperationMenu(BrowserNode* node, QList<QMenu*>& menuList)
{
    BrowserOperation* oper = static_cast<BrowserOperation*>(node);
    QMenu* nodeMenu = new QMenu();
    menuList << nodeMenu;
    Q3PtrList<BrowserNode> ImplBy;
    MenuFactory::createTitle(*nodeMenu, oper->def->definition(FALSE, TRUE));
    OperationSlots* operSlots = qobject_cast<OperationSlots*>(oper->nodeSlots.get());
    nodeMenu->insertSeparator();

    if (!oper->deletedp())
    {
        if (!oper->is_edited && (oper->get_container(UmlClass) != 0))
            nodeMenu->addAction(TR("Up"), oper->nodeSlots.get(), SLOT(OnGoUpOneLevel()))->setParent(nodeMenu);

        nodeMenu->addAction(TR("Edit"), oper->nodeSlots.get(), SLOT(OnEdit()))->setParent(nodeMenu);
        nodeMenu->addAction(TR("Add constructor initializer"), operSlots, SLOT(OnEditConstructorInitializer()))->setParent(nodeMenu);
        if(GenerationSettings::cpp_get_default_defs())
        {
            if(IsPrivateClass(node))
                nodeMenu->addAction(TR("Move to public class"), oper->nodeSlots.get(), SLOT(OnMoveToPublicClass()))->setParent(nodeMenu);
            else
                nodeMenu->addAction(TR("Move to private class"), oper->nodeSlots.get(), SLOT(OnMoveToPrivateClass()))->setParent(nodeMenu);
        }



        if (GenerationSettings::cpp_get_default_defs() && (strstr(oper->def->get_cppdef(), "${body}") != 0))
            nodeMenu->addAction(TR("Edit C++ Body"), operSlots, SLOT(OnEditCppBody()))->setParent(nodeMenu);
        if (GenerationSettings::java_get_default_defs() &&  (strstr(oper->def->get_javadef(), "${body}") != 0))
            nodeMenu->addAction(TR("Edit Java Body"), operSlots, SLOT(OnEditJavaBody()))->setParent(nodeMenu);
        if (GenerationSettings::php_get_default_defs() &&  (strstr(oper->def->get_phpdef(), "${body}") != 0))
            nodeMenu->addAction(TR("Edit Php Body"), operSlots, SLOT(OnEditPhpBody()))->setParent(nodeMenu);
        if (GenerationSettings::python_get_default_defs() &&  (strstr(oper->def->get_pythondef(), "${body}") != 0))
            nodeMenu->addAction(TR("Edit Python Body"), operSlots, SLOT(OnEditPythonBody()))->setParent(nodeMenu);


        if (((BrowserClass *)node->parent())->is_writable())
        {
                nodeMenu->addAction(TR("Add implementing activity"), operSlots, SLOT(OnAddImplementingActivity()))->setParent(nodeMenu);
                nodeMenu->addAction(TR("Add implementing state"), operSlots, SLOT(OnAddImplementingState()))->setParent(nodeMenu);
        }

        nodeMenu->addSeparator();
        nodeMenu->addAction(TR("Referenced by"), oper->nodeSlots.get(), SLOT(OnShowReferencedBy()))->setParent(nodeMenu);


        ImplBy.clear();
        BrowserActivity::compute_referenced_by(ImplBy, oper);
        BrowserState::compute_referenced_by(ImplBy, oper);

        if (!ImplBy.isEmpty())
        {
            QMenu* behaviourMenu = nodeMenu->addMenu(TR("Select implementing behavior"));
            //menuList << behaviourMenu;
            MenuFactory::createTitle(*behaviourMenu, TR("Choose behavior"));

            behaviourMenu->addSeparator();

            BrowserNode * beh;
            int rank = 10000;

            for (beh = ImplBy.first(); beh != 0; beh = ImplBy.next())
            {
                QAction* behAction = behaviourMenu->addAction(beh->full_name(TRUE), operSlots,SLOT(OnExecuteBehaviourSlot()));
                behAction->setData(rank);
            }
        }

        if ((oper->get_of == 0) && (oper->set_of == 0) && ((BrowserClass *)node->parent())->is_writable())
            nodeMenu->addAction(TR("Duplicate"), oper->nodeSlots.get(), SLOT(OnDuplicate()))->setParent(nodeMenu);


        bool editable = oper->is_read_only && (oper->edition_number == 0);

        //nodeMenu->addAction(TR("Referenced by"), oper->nodeSlots.get(), SLOT(OnShowReferencedBy()))->setParent(nodeMenu);
        if(editable)
        {
            nodeMenu->insertSeparator();
            nodeMenu->addAction(TR("Delete"), oper->nodeSlots.get(), SLOT(OnDeleteItem()))->setParent(nodeMenu);
        }
        nodeMenu->insertSeparator();
        oper->markMenu(nodeMenu);
        An<ProfiledStereotypes>()->Menu(nodeMenu, oper);
        QMenu * toolMenu = new QMenu();
        if(oper->edition_number == 0)
        {
            Tool::Menu(toolMenu, oper, oper->get_type(), oper->GetToolMenuBase());

            nodeMenu->addSeparator();
            toolMenu->setTitle(TR("Tools"));
            nodeMenu->addMenu(toolMenu);
            nodeMenu->addSeparator();
            //menuList << toolMenu;
        }
     }
    else if(!oper->is_read_only && (oper->edition_number == 0) && ((oper->get_of == 0) || !oper->get_of->deletedp()) && ((oper->set_of == 0) || !oper->set_of->deletedp()))
    {
        nodeMenu->addAction(TR("Undelete"), oper->nodeSlots.get(), SLOT(OnUndeleteItem()))->setParent(nodeMenu);
    }
    return nodeMenu;
}

