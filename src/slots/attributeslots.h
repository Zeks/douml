#ifndef ATTRIBUTESLOTS_H
#define ATTRIBUTESLOTS_H
#include <QObject>
#include "slots/nodeslots.h"
class BrowserAttribute;
class AttributeSlots : public NodeSlots
{
    Q_OBJECT
    public:
    AttributeSlots(QObject* parent = nullptr);
    ~AttributeSlots();

public slots:
    void OnAddGetOperation();
    void OnAddSetOperation();
    void OnAddGetAndSetOperations();
    void OnDuplicate();

};
#endif // ATTRIBUTESLOTS_H
