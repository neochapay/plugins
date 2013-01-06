/*
 * This file is part of Maliit Plugins
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * Copyright (C) 2012-2013 Canonical Ltd
 *
 * Contact: Mohammad Anwari <Mohammad.Anwari@nokia.com>
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

#include "setup.h"
#include "abstracttexteditor.h"
#include "abstractfeedback.h"

#include "models/keyareacontainer.h"
#include "models/key.h"
#include "models/wordcandidate.h"
#include "models/text.h"

#include "logic/layout.h"
#include "logic/layoutupdater.h"

namespace MaliitKeyboard {
namespace Setup {

void connectAll(Model::KeyAreaContainer *container,
                Logic::LayoutUpdater *updater,
                AbstractTextEditor *editor)
{
    connectContainerToLayoutUpdater(container, updater);
    connectContainerToTextEditor(container, editor);
    connectLayoutUpdaterToTextEditor(updater, editor);
}

void connectContainerToLayoutUpdater(Model::KeyAreaContainer *container,
                                     Logic::LayoutUpdater *updater)
{
    // Connect key signals to key signal handlers:
    QObject::connect(container, SIGNAL(keyPressed(Key)),
                     updater,   SLOT(onKeyPressed(Key)));

    QObject::connect(container, SIGNAL(keyLongPressed(Key)),
                     updater,   SLOT(onKeyLongPressed(Key)));

    QObject::connect(container, SIGNAL(keyReleased(Key)),
                     updater,   SLOT(onKeyReleased(Key)));

    QObject::connect(container, SIGNAL(keyAreaPressed(Logic::Layout::Panel)),
                     updater,   SLOT(onKeyAreaPressed(Logic::Layout::Panel)));

    QObject::connect(container, SIGNAL(keyAreaReleased(Logic::Layout::Panel)),
                     updater,   SLOT(onKeyAreaReleased(Logic::Layout::Panel)));

    QObject::connect(container, SIGNAL(keyEntered(Key)),
                     updater,   SLOT(onKeyEntered(Key)));

    QObject::connect(container, SIGNAL(keyExited(Key)),
                     updater,   SLOT(onKeyExited(Key)));
}

void connectContainerToTextEditor(Model::KeyAreaContainer *container,
                                  AbstractTextEditor *editor)
{
    QObject::connect(container, SIGNAL(keyPressed(Key)),
                     editor,    SLOT(onKeyPressed(Key)));

    QObject::connect(container, SIGNAL(keyReleased(Key)),
                     editor,    SLOT(onKeyReleased(Key)));

    QObject::connect(container, SIGNAL(keyEntered(Key)),
                     editor,    SLOT(onKeyEntered(Key)));

    QObject::connect(container, SIGNAL(keyExited(Key)),
                     editor,    SLOT(onKeyExited(Key)));
}

void connectLayoutToFeedback(Model::KeyAreaContainer *layout,
                             AbstractFeedback *feedback)
{
    QObject::connect(layout,   SIGNAL(keyPressed(Key)),
                     feedback, SLOT(onKeyPressed()));
    QObject::connect(layout,   SIGNAL(keyReleased(Key)),
                     feedback, SLOT(onKeyReleased()));
    QObject::connect(layout,   SIGNAL(switchLeft()),
                     feedback, SLOT(onLayoutChanged()));
    QObject::connect(layout,   SIGNAL(switchRight()),
                     feedback, SLOT(onLayoutChanged()));
    QObject::connect(layout,   SIGNAL(keyboardClosed()),
                     feedback, SLOT(onKeyboardHidden()));
}

void connectLayoutUpdaterToTextEditor(Logic::LayoutUpdater *updater,
                                      AbstractTextEditor *editor)
{
    QObject::connect(updater, SIGNAL(wordCandidateSelected(QString)),
                     editor,  SLOT(replaceAndCommitPreedit(QString)));

    QObject::connect(updater, SIGNAL(addToUserDictionary()),
                     editor,  SLOT(showUserCandidate()));

    QObject::connect(updater, SIGNAL(userCandidateSelected(QString)),
                     editor,  SLOT(addToUserDictionary(QString)));

    QObject::connect(editor,  SIGNAL(preeditEnabledChanged(bool)),
                     updater, SLOT(setWordRibbonVisible(bool)));

    QObject::connect(editor,  SIGNAL(wordCandidatesChanged(WordCandidateList)),
                     updater, SLOT(onWordCandidatesChanged(WordCandidateList)));

    QObject::connect(editor,  SIGNAL(autoCapsActivated()),
                     updater, SIGNAL(autoCapsActivated()));
}

}} // namespace Setup, MaliitKeyboard
