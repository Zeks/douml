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
