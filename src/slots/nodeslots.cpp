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

#include "slots/nodeSlots.h"
#include "browser/BrowserNode.h"
#include "ProfiledStereotypes.h"
#include "BasicData.h"
#include "ReferenceDialog.h"
#include "BrowserAttribute.h"
#include "BrowserRelation.h"
#include "OperationData.h"
#include "BrowserOperation.h"
#include "BrowserView.h"
#include "tool/ToolCom.h"
#include "tool/Tool.h"


void NodeSlots::OnDeleteItem()
{
    if (!strcmp(((BrowserNode *) node->parent())->get_data()->get_stereotype(),
                "stereotype"))
        ProfiledStereotypes::deleted(node);

    node->delete_it();
}

void NodeSlots::OnUndeleteItem()
{
    node->undelete(FALSE);

    if (!strcmp(((BrowserNode *) node->parent())->get_data()->get_stereotype(),
                "stereotype"))
        // the deletion was may be not propaged
        ProfiledStereotypes::recompute(TRUE);
}

void NodeSlots::OnShowReferencedBy()
{
    ReferenceDialog::show(node);
}


NodeSlots::NodeSlots(QObject* parent) : QObject(parent)
{

}

NodeSlots::~NodeSlots()
{
}

void NodeSlots::SetOrigin(BrowserNode * nodeLink)
{
    node = nodeLink;
}


void NodeSlots::OnMarkCurrentItem()
{
    node->toggle_mark();
}

void NodeSlots::OnUnmarkAllItems()
{
    node->unmark_all();
}

void NodeSlots::OnMoveMarkedInto()
{
    node->MoveNodes(node->marked_list, node, node);
}

void NodeSlots::OnMoveMarkedAfter()
{
    node->MoveNodes(node->marked_list, (BrowserNode *) node->parent(), node);
}

void NodeSlots::OnDuplicateMarkedInto()
{
    for (BrowserNode* bn = node->marked_list.last();
         bn != 0;
         bn = node->marked_list.prev())
    {
        BrowserNode* getOperCopy = nullptr;
        BrowserNode* setOperCopy = nullptr;
        if((BrowserNode *) bn->parent()  != node)
        {
            BrowserNode* nodeCopy = bn->duplicate(node);
            //node->move(nodeCopy, 0);
            node->move(nodeCopy, node);
            nodeCopy->select_in_browser();

            if (nodeCopy->get_type() == UmlAttribute)
            {
                BrowserAttribute* asAttribute =  dynamic_cast<BrowserAttribute*>(bn);
                getOperCopy = ((BrowserNode *) asAttribute->get_get_oper())->duplicate(node);
                setOperCopy = ((BrowserNode *) asAttribute->get_set_oper())->duplicate(node);
                node->move(getOperCopy, nodeCopy);
                node->move(setOperCopy, nodeCopy);

                ((BrowserAttribute *) nodeCopy)->set_get_oper((BrowserOperation *) getOperCopy);
                ((BrowserAttribute *) nodeCopy)->set_set_oper((BrowserOperation *) setOperCopy);
            }
            else if(bn->get_type() >= UmlAggregation &&  bn->get_type() <= UmlDirectionalAggregationByValue)
            {
                BrowserRelation* asRelation =  dynamic_cast<BrowserRelation*>(bn);
                BrowserNode* getOper=((BrowserNode *) asRelation->get_get_oper());
                BrowserNode* setOper=((BrowserNode *) asRelation->get_get_oper());
                if(getOper)
                    getOperCopy = getOper->duplicate(node);
                if(setOper)
                    setOperCopy = setOper->duplicate(node);
                if(getOperCopy)
                {
                    node->move(getOperCopy, nodeCopy);
                    ((BrowserRelation *) nodeCopy)->set_get_oper((BrowserOperation *) getOperCopy);
                }
                if(setOperCopy)
                {
                    node->move(setOperCopy, nodeCopy);
                    ((BrowserRelation *) nodeCopy)->set_set_oper((BrowserOperation *) setOperCopy);
                }
            }
            if((BrowserOperation*)setOperCopy)
            {
                ((OperationData*)((BrowserOperation*)setOperCopy)->get_data())->set_get_or_set(true);
                ((BrowserOperation*)setOperCopy)->set_set_of(nodeCopy);
            }
            if((BrowserOperation*)getOperCopy)
            {
                ((OperationData*)((BrowserOperation*)setOperCopy)->get_data())->set_get_or_set(true);
                ((BrowserOperation*)getOperCopy)->set_get_of(nodeCopy);
            }
        }
    }
}

void NodeSlots::OnDuplicateMarkedAfter()
{
    BrowserNode * p = (BrowserNode *) node->parent();

    for (BrowserNode * bn = node->marked_list.last(); bn != 0; bn = node->marked_list.prev())
        p->move(bn->duplicate(p), node);
}

void NodeSlots::OnGoUpOneLevel()
{
    BrowserView::deselect(node);
    BrowserView::select(node->get_container(UmlClass));
}

void NodeSlots::OnEdit()
{
    node->open(true);
}

void NodeSlots::OnExecuteToolOfChoice()
{
    QAction* action = static_cast<QAction*>(sender());
    int rank = action->data().toInt();
    ToolCom::run(Tool::command(rank - node->GetToolMenuBase()), node);
}
