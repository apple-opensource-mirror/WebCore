/*
 * Copyright (C) 2006, 2007, 2008 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef Icon_h
#define Icon_h

#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/Forward.h>
#include <wtf/Vector.h>

#include "ImageSource.h"
#include <CoreGraphics/CoreGraphics.h>
#include <wtf/RetainPtr.h>

namespace WebCore {

class GraphicsContext;
class IntRect;
    
class Icon : public RefCounted<Icon> {
public:
    static PassRefPtr<Icon> createIconForFiles(const Vector<String>& filenames);
    static PassRefPtr<Icon> createIconForImage(NativeImagePtr);

    ~Icon();

    void paint(GraphicsContext*, const IntRect&);

#if PLATFORM(WIN)
    static PassRefPtr<Icon> create(HICON hIcon) { return adoptRef(new Icon(hIcon)); }
#elif PLATFORM(CHROMIUM)
    static PassRefPtr<Icon> create(PassRefPtr<PlatformIcon> icon) { return adoptRef(new Icon(icon)); }
#endif

private:
    Icon(CGImageRef);
    RetainPtr<CGImageRef> m_cgImage;
};

}

#endif
