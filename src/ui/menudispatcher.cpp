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
#include "ProfiledStereotypes.h"
#include "tool/Tool.h"
#include "menufactory.h"
#include "data/AttributeData.h"
#include "translate.h"
#include "slots/nodeslots.h"
#include "slots/attributeslots.h"

MenuDispatcher::MenuDispatcher()
{
    AddMenu(TypeIdentifier<BrowserAttribute>::id(), AttributeMenu);
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
        delete menu;
        menu = nullptr;
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
            menuList << toolMenu;
        }
     }
    else
        nodeMenu->addAction(TR("Undelete"), attr->nodeSlots.get(), SLOT(OnUndeleteItem()))->setParent(nodeMenu);
    return nodeMenu;
}
