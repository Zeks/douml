#ifndef SLOTSDISPATCHER_H
#define SLOTSDISPATCHER_H
#include <QHash>
#include <functional>
class NodeSlots;

NodeSlots* GetSlotsObject(unsigned);
QHash<unsigned int, std::function<NodeSlots*(void)>> SetupDispatchers();

#endif // SLOTSDISPATCHER_H
