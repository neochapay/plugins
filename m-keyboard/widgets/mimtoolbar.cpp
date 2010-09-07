/* * This file is part of meego-keyboard *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */



#include "mimtoolbar.h"
#include "layoutsmanager.h"
#include "mhardwarekeyboard.h"
#include "mvirtualkeyboardstyle.h"
#include "mtoolbarbutton.h"
#include "mtoolbarlabel.h"

#include <mtoolbardata.h>
#include <mtoolbaritem.h>

#include <MNamespace>
#include <MButton>
#include <QKeySequence>
#include <QGraphicsLinearLayout>
#include <QDebug>
#include <MSceneManager>
#include <mplainwindow.h>
#include <mreactionmap.h>
#include <MScalableImage>

namespace
{
    const QString ObjectNameToolbar("MImToolbar");
    const QString ObjectNameToolbarLeft("VirtualKeyboardToolbarLeft");
    const QString ObjectNameToolbarRight("VirtualKeyboardToolbarRight");
    const QString NameToolbarCopyPasteButton("VirtualKeyboardCopyPasteButton");
};

MImToolbar::MImToolbar(QGraphicsWidget *parent)
    : MStylableWidget(parent),
      textSelected(false),
      copyPasteItem(new MToolbarItem(NameToolbarCopyPasteButton, MInputMethod::ItemButton)),
      leftBar(true, this),
      rightBar(true, this),
      shiftState(ModifierClearState),
      fnState(ModifierClearState)
{
    leftBar.setObjectName(ObjectNameToolbarLeft);
    rightBar.setObjectName(ObjectNameToolbarRight);
    setObjectName(ObjectNameToolbar);

    setupLayout();

    connect(this, SIGNAL(visibleChanged()), this, SLOT(updateVisibility()));
    connect(MTheme::instance(), SIGNAL(themeChangeCompleted()),
            this, SLOT(updateFromStyle()));
}

MImToolbar::~MImToolbar()
{
}

void MImToolbar::setupLayout()
{
    QGraphicsLinearLayout *mainLayout = new QGraphicsLinearLayout(Qt::Vertical, this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QGraphicsLinearLayout *rowLayout = new QGraphicsLinearLayout(Qt::Horizontal, mainLayout);

    setupRowLayout(rowLayout, &leftBar, &rightBar);
    mainLayout->insertItem(0, rowLayout);

    resize(geometry().width(), layout()->preferredHeight());
}

QRegion MImToolbar::region() const
{
    QRegion region;

    if (isVisible()) {
        if (currentToolbar && currentToolbar->isCustom()) {
            region = QRegion(mapRectToScene(rect()).toRect());
        } else if (rightBar.isVisible()) {
            region = QRegion(mapRectToScene(rightBar.geometry()).toRect());
        }
    }
    return region;
}

void MImToolbar::handleButtonClick(MToolbarItem *item)
{
    if (!item || item->type() != MInputMethod::ItemButton)
        return;

    foreach(QSharedPointer<const MToolbarItemAction> action, item->actions()) {
        switch (action->type()) {
        case MInputMethod::ActionSendKeySequence:
            sendKeySequence(action->keys());
            break;
        case MInputMethod::ActionSendString:
            sendStringRequest(action->text());
            break;
        case MInputMethod::ActionSendCommand:
            //TODO:not support yet
            break;
        case MInputMethod::ActionCopy:
            emit copyPasteRequest(InputMethodCopy);
            break;
        case MInputMethod::ActionPaste:
            emit copyPasteRequest(InputMethodPaste);
            break;
        case MInputMethod::ActionShowGroup:
            showGroup(action->group());
            break;
        case MInputMethod::ActionHideGroup:
            hideGroup(action->group());
            break;
        case MInputMethod::ActionClose:
            emit closeKeyboardRequest();
            break;
        case MInputMethod::ActionUndefined:
        case MInputMethod::ActionCopyPaste:
            break;
        }
    }
}

void MImToolbar::setSelectionStatus(bool selection)
{
    if (textSelected != selection) {
        textSelected = selection;
        if (isVisible())
            updateVisibility();
    }
}

void MImToolbar::updateVisibility()
{
    if (currentToolbar) {
        foreach (const QSharedPointer<MToolbarItem> item, currentToolbar->allItems())
        {
            if ((item->showOn() == MInputMethod::VisibleAlways)
                    || (textSelected && item->showOn() == MInputMethod::VisibleWhenSelectingText)
                    || (!textSelected && item->hideOn() == MInputMethod::VisibleWhenSelectingText)) {
                item->setVisible(true);
            }

            if ((!textSelected && item->showOn() == MInputMethod::VisibleWhenSelectingText)
                    || (textSelected && item->hideOn() == MInputMethod::VisibleWhenSelectingText)) {
                item->setVisible(false);
            }
        }
    }
    arrangeWidgets();
}

void MImToolbar::updateFromStyle()
{
    prepareGeometryChange(); // notify scene about changes in bounding rect
}

void MImToolbar::loadCustomWidgets()
{
    if (!currentToolbar) {
        return;
    }

    const M::Orientation orientation = MPlainWindow::instance()->sceneManager()->orientation();
    QSharedPointer<const MToolbarLayout> layout = currentToolbar->layout(orientation);
    QGraphicsLinearLayout *mainLayout = static_cast<QGraphicsLinearLayout*>(this->layout());

    if (!mainLayout) {
        qCritical() << __PRETTY_FUNCTION__ << "Layout does not exist";
    }

    QList<QSharedPointer<const MToolbarRow> > rows = layout->rows();

    //create additional rows if necessary
    for (int n = 0; n < rows.count() - 1; ++n) {
        QSharedPointer<const MToolbarRow> row = rows[n];
        QGraphicsLinearLayout *rowLayout = new QGraphicsLinearLayout(Qt::Horizontal, mainLayout);
        WidgetBar *leftWidget  = new WidgetBar(true, this);
        WidgetBar *rightWidget = new WidgetBar(true, this);

        leftWidget->setObjectName(ObjectNameToolbarLeft);
        rightWidget->setObjectName(ObjectNameToolbarRight);
        setupRowLayout(rowLayout, leftWidget, rightWidget);
        mainLayout->insertItem(n, rowLayout);

        foreach (QSharedPointer<MToolbarItem> item , row->items()) {
            createAndAppendWidget(item, leftWidget, rightWidget);
        }
    }

    //add custom items to bottom row
    QSharedPointer<const MToolbarRow> row = rows.last();
    foreach (QSharedPointer<MToolbarItem> item, row->items()) {
        createAndAppendWidget(item, &leftBar, &rightBar);
    }

    mainLayout->invalidate();
}

void MImToolbar::createAndAppendWidget(QSharedPointer<MToolbarItem> item,
                                       WidgetBar *leftWidget,
                                       WidgetBar *rightWidget)
{
    MWidget *widget = 0;
    WidgetBar *sidebar = 0;

    if (item->alignment() == Qt::AlignLeft) {
        sidebar = leftWidget;
    } else {
        sidebar = rightWidget;
    }
    if (item->type() == MInputMethod::ItemButton) {
        widget = new MToolbarButton(item, sidebar);

        connect(widget, SIGNAL(clicked(MToolbarItem*)),
                this, SLOT(handleButtonClick(MToolbarItem*)));
    } else {
        widget = new MToolbarLabel(item, sidebar);
    }
    customWidgets.append(widget);
    sidebar->append(widget, item->isVisible());
    if (sidebar->count() == 1) {
        sidebar->show();
    }
}

void MImToolbar::setupRowLayout(QGraphicsLinearLayout *rowLayout,
                                WidgetBar *leftWidget,
                                WidgetBar *rightWidget)
{
    rowLayout->setContentsMargins(0, 0, 0, 0);

    // Empty button bars are hidden.
    leftWidget->hide();
    rightWidget->hide();
    // Add the left and right side WidgetBar widgets with a stretch item in between.
    rowLayout->addItem(leftWidget);
    rowLayout->addStretch();
    rowLayout->addItem(rightWidget);

    rowLayout->setAlignment(leftWidget, Qt::AlignBottom);
    rowLayout->setAlignment(rightWidget, Qt::AlignBottom);
}

void MImToolbar::unloadCustomWidgets()
{
    QGraphicsLinearLayout *mainLayout = static_cast<QGraphicsLinearLayout*>(layout());
    QList<QGraphicsLinearLayout*> rows;

    if (!mainLayout) {
        qCritical() << __PRETTY_FUNCTION__ << "Layout does not exist";
    }

    //delete all dynamically created rows
    for (int n = 0; n < mainLayout->count() - 1; ++n) {
        QGraphicsLinearLayout *rowLayout = dynamic_cast<QGraphicsLinearLayout*>(mainLayout->itemAt(n));

        if (rowLayout) {
            WidgetBar *leftBar  = dynamic_cast<WidgetBar*>(rowLayout->itemAt(0));
            WidgetBar *rightBar = dynamic_cast<WidgetBar*>(rowLayout->itemAt(1));

            delete leftBar;
            delete rightBar;
            rows << rowLayout;
        }
    }

    qDeleteAll(rows);
    qDeleteAll(customWidgets);
    customWidgets.clear();
    leftBar.cleanup();
    rightBar.cleanup();
}

void MImToolbar::arrangeWidgets()
{
    if (!layout()) {
        qCritical() << __PRETTY_FUNCTION__ << "Layout does not exist";
    }

    if (isVisible()) {
        for (int n = 0; n < layout()->count(); ++n) {
            QGraphicsLayout *rowLayout = dynamic_cast<QGraphicsLayout*>(layout()->itemAt(n));
            if (rowLayout) {
                rowLayout->invalidate();
                rowLayout->activate();
            }
        }

        layout()->invalidate();
        layout()->activate();
        resize(geometry().width(), layout()->preferredHeight());
    }

    emit availabilityChanged((rightBar.count() != 0) || (leftBar.count() != 0));
    emit regionUpdated();
}

void MImToolbar::showGroup(const QString &group)
{
    bool changed = false;

    if (!currentToolbar) {
        return;
    }

    foreach (const QSharedPointer<MToolbarItem> item, currentToolbar->allItems())
    {
        if (item->group() == group && !(item->isVisible())) {
            item->setVisible(true);
            changed = true;
        }
    }

    if (changed) {
        arrangeWidgets();
    }
}

void MImToolbar::hideGroup(const QString &group)
{
    bool changed = false;

    if (!currentToolbar) {
        return;
    }

    foreach (const QSharedPointer<MToolbarItem> item, currentToolbar->allItems())
    {
        if (item->group() == group && item->isVisible()) {
            item->setVisible(false);
            changed = true;
        }
    }

    if (changed) {
        arrangeWidgets();
    }
}

void MImToolbar::sendKeySequence(const QString &keys)
{
    const QKeySequence keysequence(keys);
    //translate the keys string to QKeyEvent by QKeySequence
    if (!keysequence.isEmpty()) {
        for (uint i = 0; i < keysequence.count(); i++) {
            int key = keysequence[i];
            Qt::KeyboardModifiers modify = keyModifiers(key);
            key -= modify;
            QString text;
            if (modify == Qt::NoModifier || modify == Qt::ShiftModifier)
                text = QString(key);
            //send both KeyPress and KeyRelease
            QKeyEvent press(QEvent::KeyPress, key, modify, text);
            emit sendKeyEventRequest(press);
            QKeyEvent release(QEvent::KeyRelease, key, modify, text);
            emit sendKeyEventRequest(release);
        }
    }
}

Qt::KeyboardModifiers MImToolbar::keyModifiers(int key) const
{
    Qt::KeyboardModifiers modify = Qt::NoModifier;
    if (key & Qt::CTRL)
        modify |= Qt::ControlModifier;
    if (key & Qt::ALT)
        modify |= Qt::AltModifier;
    if (key & Qt::SHIFT)
        modify |= Qt::ShiftModifier;
    if (key & Qt::META)
        modify |= Qt::MetaModifier;
    return modify;
}

void MImToolbar::setShapedMode(bool shaped)
{
    if (shaped) {
        style().setModeShapedToolbar();
    } else {
        style().setModeFullToolbar();
    }
    rightBar.setShapedMode(shaped);
}

void MImToolbar::showToolbarWidget(QSharedPointer<const MToolbarData> toolbar)
{
    if (toolbar == currentToolbar) {
        return;
    }
    unloadCustomWidgets();

    currentToolbar = toolbar;
    loadCustomWidgets();

    setShapedMode(!toolbar->isCustom());

    if (isVisible())
        updateVisibility();
}

void MImToolbar::hideToolbarWidget()
{
    currentToolbar.clear();
    unloadCustomWidgets();
    arrangeWidgets();
    setShapedMode(true);
}

void MImToolbar::removeItem(MWidget *widget)
{
    leftBar.remove(widget);
    rightBar.remove(widget);

    if (leftBar.count() == 0) {
        leftBar.hide();
    }
    if (rightBar.count() == 0) {
        rightBar.hide();
    }

    if (!layout()) {
        qCritical() << __PRETTY_FUNCTION__ << "Layout does not exist";
    }

    layout()->invalidate();
    layout()->activate();
}

void MImToolbar::redrawReactionMaps()
{
    if (!layout()) {
        qCritical() << __PRETTY_FUNCTION__ << "Layout does not exist";
    }

    foreach(QGraphicsView * view, scene()->views()) {
        MReactionMap *reactionMap = MReactionMap::instance(view);
        if (!reactionMap) {
            continue;
        }

        // TODO: support for translucent keyboard
        reactionMap->setTransform(this, view);
        reactionMap->setInactiveDrawingValue();
        reactionMap->fillRectangle(boundingRect());

        // Draw all widgets geometries.
        reactionMap->setReactiveDrawingValue();

        for (int n = 0; n < layout()->count(); ++n) {
            QGraphicsLinearLayout *row = dynamic_cast<QGraphicsLinearLayout*>(layout()->itemAt(n));

            if (row) {
                for (int j = 0; j < row->count(); ++j) {
                    WidgetBar *sidebar = dynamic_cast<WidgetBar*>(row->itemAt(j));
                    if (!sidebar || !sidebar->isVisible()) {
                        continue;
                    }

                    for (int i = 0; i < sidebar->count(); ++i) {
                        if (dynamic_cast<MToolbarLabel*>(sidebar->widgetAt(i))) {
                            reactionMap->setInactiveDrawingValue();
                        } else if (sidebar->widgetAt(i) && sidebar->widgetAt(i)->isVisible()) {
                            reactionMap->setReactiveDrawingValue();
                        }

                        MWidget *widget = sidebar->widgetAt(i);
                        reactionMap->setTransform(widget, view);
                        reactionMap->fillRectangle(widget->boundingRect());
                    }
                }
            }
        }
    }
}

void MImToolbar::finalizeOrientationChange()
{
    //use brute force: destroy everything and construct it again
    unloadCustomWidgets();
    loadCustomWidgets();

    if (isVisible()) {
        blockSignals(true);
        arrangeWidgets();
        blockSignals(false);
    }
}

QRectF MImToolbar::boundingRect() const
{
    return QRectF(-style()->marginLeft(), -style()->marginTop(),
                  size().width() + style()->marginLeft() + style()->marginRight(),
                  size().height() + style()->marginTop() + style()->marginBottom());
}

