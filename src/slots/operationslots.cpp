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


#include "slots/operationslots.h"

#include "browser/BrowserNode.h"
#include "ProfiledStereotypes.h"
#include "BasicData.h"
#include "data/ActivityData.h"
#include "data/StateData.h"
#include "ReferenceDialog.h"
#include "BrowserAttribute.h"
#include "BrowserRelation.h"
#include "BrowserActivity.h"
#include "OperationData.h"
#include "BrowserOperation.h"
#include "BrowserView.h"
#include "BrowserState.h"
#include "BrowserClass.h"
#include "tool/ToolCom.h"
#include "tool/Tool.h"


void OperationSlots::OnDuplicate()
{
    ((BrowserClass *) node->parent())->duplicate_operation(static_cast<BrowserOperation*>(node));
    SetModified();
}

void OperationSlots::OnEditConstructorInitializer()
{
    static_cast<BrowserOperation*>(node)->AddConstructorInitalizer();
    SetModified();
}

void OperationSlots::OnEditCppBody()
{

    operationNode->def->edit(CppView);
    SetModified();
}

void OperationSlots::OnEditJavaBody()
{
    operationNode->def->edit(JavaView);
    SetModified();
}

void OperationSlots::OnEditPhpBody()
{
    operationNode->def->edit(PhpView);
    SetModified();
}

void OperationSlots::OnEditPythonBody()
{
    operationNode->def->edit(PythonView);
    SetModified();
}

void OperationSlots::OnAddImplementingActivity()
{
    BrowserNode * bn = node;

    do {
        bn = (BrowserNode *) bn->parent();
    }
    while (bn->get_type() == UmlClass);

    BrowserActivity * a = BrowserActivity::add_activity(operationNode);

    if (a != 0) {
        ((ActivityData *) a->get_data())->set_specification(operationNode);
        operationNode->package_modified();
    }
    SetModified();
}

void OperationSlots::OnAddImplementingState()
{
    BrowserNode * bn = node;

    do {
        bn = (BrowserNode *) bn->parent();
    }
    while (bn->get_type() == UmlClass);

    BrowserState * st = BrowserState::add_state(operationNode, TRUE);

    if (st != 0) {
        ((StateData *) st->get_data())->set_specification(operationNode);
        operationNode->package_modified();
    }
    SetModified();
}


OperationSlots::OperationSlots(QObject *parent) : NodeSlots(parent)
{
    operationNode = static_cast<BrowserOperation*>(node);
}

OperationSlots::~OperationSlots()
{
}

void OperationSlots::SetModified()
{
    ((BrowserNode *) operationNode->parent())->modified();
    operationNode->package_modified();
}


void OperationSlots::OnExecuteBehaviourSlot()
{
    Q3PtrList<BrowserNode> ImplBy;
    BrowserActivity::compute_referenced_by(ImplBy, operationNode);
    BrowserState::compute_referenced_by(ImplBy, operationNode);

    QAction* action = static_cast<QAction*>(sender());
    int rank = action->data().toInt();

    ImplBy.at(rank - 10000)->select_in_browser();

}
