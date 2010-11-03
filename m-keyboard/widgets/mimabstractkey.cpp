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

#include "mimabstractkey.h"

QList<MImAbstractKey *> MImAbstractKey::activeKeys;

MImAbstractKey::~MImAbstractKey()
{
    activeKeys.removeAll(this);
}

MImAbstractKey* MImAbstractKey::lastActiveKey()
{
    return (activeKeys.isEmpty() ? 0 : activeKeys.last());
}

void MImAbstractKey::resetActiveKeys()
{
    while (!activeKeys.isEmpty()) {
        MImAbstractKey *key = activeKeys.takeFirst();
        key->resetTouchPointCount();
    }
}

QList<const MImAbstractKey *> MImAbstractKey::filterActiveKeys(bool (predicate)(const MImAbstractKey *))
{
    QList<const MImAbstractKey *> result;

    foreach(MImAbstractKey *key, activeKeys) {
        if (predicate(key)) {
            result.append(key);
        }
    }

    return result;
}
