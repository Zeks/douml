#include "slots/attributeslots.h"

#include "browser/BrowserNode.h"
#include "ProfiledStereotypes.h"
#include "BasicData.h"
#include "ReferenceDialog.h"
#include "BrowserAttribute.h"
#include "BrowserRelation.h"
#include "OperationData.h"
#include "BrowserOperation.h"
#include "BrowserView.h"
#include "BrowserClass.h"
#include "tool/ToolCom.h"
#include "tool/Tool.h"

void AttributeSlots::OnAddGetOperation()
{
    static_cast<BrowserAttribute*>(node)->add_get_oper();
}


void AttributeSlots::OnAddSetOperation()
{
    static_cast<BrowserAttribute*>(node)->add_set_oper();
}


void AttributeSlots::OnAddGetAndSetOperations()
{
    static_cast<BrowserAttribute*>(node)->add_get_oper();
    static_cast<BrowserAttribute*>(node)->add_set_oper();
}

void AttributeSlots::OnDuplicate()
{
    ((BrowserClass *) node->parent())->duplicate_attribute(static_cast<BrowserAttribute*>(node));
}


AttributeSlots::AttributeSlots(QObject *parent) : NodeSlots(parent)
{

}

AttributeSlots::~AttributeSlots()
{
}
