#ifndef MENUDISPATCHER_H
#define MENUDISPATCHER_H
#include "SingletonHolder.h"
#include <QMenu>
class BrowserNode;
class MenuDispatcher
{
public:
    MenuDispatcher();
    ~MenuDispatcher();
    QMenu* GetMenu(uint classID, BrowserNode*);
    void AddMenu(uint classID, std::function<QMenu*(BrowserNode*)> menuCreator);

private:
    void DeleteLastMenu();
    QHash<uint, std::function<QMenu*(BrowserNode*)>> menuCreators;
    QMenu* lastMenu;
};
BIND_TO_SELF_SINGLE(MenuDispatcher);


QMenu* AttributeMenu(BrowserNode*node);

#endif // MENUDISPATCHER_H
