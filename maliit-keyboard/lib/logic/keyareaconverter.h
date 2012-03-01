/*
 * This file is part of Maliit Plugins
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
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

#ifndef MALIIT_KEYBOARD_KEYAREACONVERTER_H
#define MALIIT_KEYBOARD_KEYAREACONVERTER_H

#include "models/layout.h"
#include <QPoint>

namespace MaliitKeyboard {

class Style;
class KeyboardLoader;
class KeyArea;
class Key;

class KeyAreaConverter
{
private:
    Style * const m_style;
    KeyboardLoader * const m_loader;
    QPoint m_anchor;

public:
    explicit KeyAreaConverter(Style *style,
                              KeyboardLoader *loader,
                              const QPoint &anchor);
    virtual ~KeyAreaConverter();

    virtual KeyArea keyArea(Layout::Orientation orientation,
                            Layout::Alignment alignment) const;
    virtual KeyArea nextKeyArea(Layout::Orientation orientation,
                                Layout::Alignment alignment) const;
    virtual KeyArea previousKeyArea(Layout::Orientation orientation,
                                    Layout::Alignment alignment) const;

    virtual KeyArea shiftedKeyArea(Layout::Orientation orientation,
                                   Layout::Alignment alignment) const;
    virtual KeyArea symbolsKeyArea(Layout::Orientation orientation,
                                   Layout::Alignment alignment,
                                   int page = 0) const;
    virtual KeyArea deadKeyArea(Layout::Orientation orientation,
                                Layout::Alignment alignment,
                                const Key &dead) const;
    virtual KeyArea shiftedDeadKeyArea(Layout::Orientation orientation,
                                       Layout::Alignment alignment,
                                       const Key &dead) const;
    virtual KeyArea extendedKeyArea(Layout::Orientation orientation,
                                    Layout::Alignment alignment,
                                    const Key &key) const;
    virtual KeyArea numberKeyArea(Layout::Orientation orientation,
                                  Layout::Alignment alignment) const;
    virtual KeyArea phoneNumberKeyArea(Layout::Orientation orientation,
                                       Layout::Alignment alignment) const;
};

} // namespace MaliitKeyboard

#endif // MALIIT_KEYBOARD_KEYAREACONVERTER_H