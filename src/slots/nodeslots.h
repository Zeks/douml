#ifndef NODESLOTS_H
#define NODESLOTS_H
#include <QObject>
class BrowserNode;
class NodeSlots : public QObject
{
    Q_OBJECT
    public:
    NodeSlots(QObject* parent = nullptr);
    virtual ~NodeSlots();
    void SetOrigin(BrowserNode*);
    BrowserNode* node = nullptr;
public slots:
    void OnMarkCurrentItem();
    void OnUnmarkAllItems();
    void OnMoveMarkedInto();
    void OnMoveMarkedAfter();
    void OnDuplicateMarkedInto();
    void OnDuplicateMarkedAFter();
    void OnGoUpOneLevel();
    void OnEdit();
    void OnExecuteToolOfChoice();
    void OnDeleteItem();
    void OnUndeleteItem();
    void OnShowReferencedBy();

};

#endif // NODESLOTS_H
