/*
 * This file is part of Maliit Plugins
 *
 * Copyright (C) 2012 Openismus GmbH. All rights reserved.
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this list
 * of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * Neither the name of Nokia Corporation nor the names of its contributors may be
 * used to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "eventhandler.h"
#include "layoutupdater.h"
#include "models/layout.h"

namespace MaliitKeyboard {
namespace Logic {

class EventHandlerPrivate
{
public:
    Model::Layout * const layout;
    LayoutUpdater * const updater;

    explicit EventHandlerPrivate(Model::Layout * const new_layout,
                                 LayoutUpdater * const new_updater);
};


EventHandlerPrivate::EventHandlerPrivate(Model::Layout *const new_layout,
                                         LayoutUpdater *const new_updater)
    : layout(new_layout)
    , updater(new_updater)
{
    Q_ASSERT(new_layout != 0);
    Q_ASSERT(new_updater != 0);
}


//! \brief Performs event handling for Model::Layout instance, using a LayoutUpdater instance.
//!
//! Does not take ownership of either layout or updater.
EventHandler::EventHandler(Model::Layout * const layout,
                           LayoutUpdater * const updater,
                           QObject *parent)
    : QObject(parent)
    , d_ptr(new EventHandlerPrivate(layout, updater))
{}


EventHandler::~EventHandler()
{}


void EventHandler::onExtendedKeysShown(const Key &key)
{
    Q_D(EventHandler);
    d->updater->onExtendedKeysShown(key);
}


void EventHandler::onEntered(int index)
{
    Q_D(EventHandler);

    const QVector<Key> &keys(d->layout->keyArea().keys());
    const Key &key(index < keys.count()
                   ? keys.at(index) : Key());

    d->updater->onKeyEntered(key);
    Q_EMIT keyEntered(key);
}


void EventHandler::onExited(int index)
{
    Q_D(EventHandler);

    const QVector<Key> &keys(d->layout->keyArea().keys());
    const Key &key(index < keys.count()
                   ? keys.at(index) : Key());

    d->updater->onKeyExited(key);
    Q_EMIT keyExited(key);
}


void EventHandler::onPressed(int index)
{
    Q_D(EventHandler);

    const QVector<Key> &keys(d->layout->keyArea().keys());
    const Key &key(index < keys.count()
                   ? keys.at(index) : Key());
    const Key pressed_key(d->updater
                          ? d->updater->modifyKey(key, KeyDescription::PressedState) : Key());

    d->layout->replaceKey(index, pressed_key);

    d->updater->onKeyPressed(pressed_key);
    Q_EMIT keyPressed(pressed_key);
}


void EventHandler::onReleased(int index)
{
    Q_D(EventHandler);

    const QVector<Key> &keys(d->layout->keyArea().keys());
    const Key &key(index < keys.count()
                   ? keys.at(index) : Key());
    const Key normal_key(d->updater
                         ? d->updater->modifyKey(key, KeyDescription::NormalState) : Key());

    d->layout->replaceKey(index, normal_key);
    d->updater->onKeyReleased(normal_key);

    Q_EMIT keyReleased(normal_key);
}


void EventHandler::onPressAndHold(int index)
{
    Q_D(EventHandler);

    const QVector<Key> &keys(d->layout->keyArea().keys());
    const Key &key(index < keys.count()
                   ? keys.at(index) : Key());

    // FIXME: long-press on space needs to work again to save words to dictionary!
    if (key.hasExtendedKeys()) {
        Q_EMIT extendedKeysShown(key);
    }

    Q_EMIT keyLongPressed(key);
}


}} // namespace Logic, MaliitKeyboard