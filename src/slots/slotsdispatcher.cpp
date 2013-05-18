#include "slots/slotsdispatcher.h"
#include "slots/nodeSlots.h"
#include "TypeIdentifier.h"
#include "browser/BrowserAttribute.h"
#include "browser/BrowserOperation.h"
#include "slots/attributeslots.h"
#include "slots/operationslots.h"
#include "slots/nodeSlots.h"
#include <QHash>
#include <functional>



NodeSlots *GetSlotsObject(unsigned objectType)
{
    static auto dispatchers = SetupDispatchers();
    if(dispatchers.contains(objectType))
        return dispatchers[objectType]();
    else
        return dispatchers[TypeIdentifier<BrowserNode>::id()]();
}


QHash<unsigned int, std::function<NodeSlots*(void)>> SetupDispatchers()
{
    QHash<unsigned int, std::function<NodeSlots*(void)> > result;
    result.insert(TypeIdentifier<BrowserOperation>::id(), [](){return new OperationSlots();});
    result.insert(TypeIdentifier<BrowserAttribute>::id(), [](){return new AttributeSlots();});
    result.insert(TypeIdentifier<BrowserNode>::id(), [](){return new NodeSlots();});
    return result;
}
