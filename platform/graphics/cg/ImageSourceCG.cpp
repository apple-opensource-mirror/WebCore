/*
 * Copyright (C) 2004, 2005, 2006 Apple Computer, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "ImageSource.h"
#include "SharedBuffer.h"

#if PLATFORM(CG)

#include "IntSize.h"
#include <CoreGraphics/CoreGraphics.h>
#include <ImageIO/ImageIO.h>
#include <ImageIO/CGImageSourcePrivate.h>

namespace WebCore {

ImageSource::ImageSource()
    : m_decoder(0)
    , m_baseSubsampling(0)
    , m_isProgressive(false)
{
}

ImageSource::~ImageSource()
{
    clear();
}

void ImageSource::clear()
{
    if (m_decoder) {
        CFRelease(m_decoder);
        m_decoder = 0;
    }
}

const CFStringRef kCGImageSourceShouldPreferRGB32 = CFSTR("kCGImageSourceShouldPreferRGB32");

CFDictionaryRef ImageSource::imageSourceOptions(int requestedSubsampling) const
{
    static CFDictionaryRef options[4] = {NULL, NULL, NULL, NULL};
    int subsampling = std::min(3, m_isProgressive ? 0 : (requestedSubsampling + m_baseSubsampling));
    
    if (!options[subsampling]) {
        int subsampleInt = 1 << subsampling;  // [0..3] => [1, 2, 4, 8]
        CFNumberRef subsampleNumber = CFNumberCreate(NULL,  kCFNumberIntType,  &subsampleInt);
        const void *keys[3] = { kCGImageSourceShouldCache, kCGImageSourceShouldPreferRGB32, kCGImageSourceSubsampleFactor };
        const void *values[3] = { kCFBooleanTrue, kCFBooleanTrue, subsampleNumber };
        options[subsampling] = CFDictionaryCreate(NULL, keys, values, 3,
            &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
        CFRelease(subsampleNumber);
    }

    return options[subsampling];
}

bool ImageSource::initialized() const
{
    return m_decoder;
}

void ImageSource::setData(SharedBuffer* data, bool allDataReceived)
{
    if (!m_decoder)
        m_decoder = CGImageSourceCreateIncremental(NULL);
#if PLATFORM(MAC)
    // On Mac the NSData inside the SharedBuffer can be secretly appended to without the SharedBuffer's knowledge.  We use SharedBuffer's ability
    // to wrap itself inside CFData to get around this, ensuring that ImageIO is really looking at the SharedBuffer.
    CFDataRef cfData = data->createCFData();
#else
    // If no NSData is available, then we know SharedBuffer will always just be a vector.  That means no secret changes can occur to it behind the
    // scenes.  We use CFDataCreateWithBytesNoCopy in that case.
    CFDataRef cfData = CFDataCreateWithBytesNoCopy(0, reinterpret_cast<const UInt8*>(data->data()), data->size(), kCFAllocatorNull);
#endif
    CGImageSourceUpdateData(m_decoder, cfData, allDataReceived);
    CFRelease(cfData);
}

bool ImageSource::isSizeAvailable()
{
    bool result = false;
    CGImageSourceStatus imageSourceStatus = CGImageSourceGetStatus(m_decoder);

    // Ragnaros yells: TOO SOON! You have awakened me TOO SOON, Executus!
    if (imageSourceStatus >= kCGImageStatusIncomplete) {
        CFDictionaryRef image0Properties = CGImageSourceCopyPropertiesAtIndex(m_decoder, 0, imageSourceOptions());
        if (image0Properties) {
            CFNumberRef widthNumber = (CFNumberRef)CFDictionaryGetValue(image0Properties, kCGImagePropertyPixelWidth);
            CFNumberRef heightNumber = (CFNumberRef)CFDictionaryGetValue(image0Properties, kCGImagePropertyPixelHeight);
            result = widthNumber && heightNumber;
            CFRelease(image0Properties);
        }
    }
    
    return result;
}

IntSize ImageSource::size() const
{
    IntSize result;
    CFDictionaryRef properties = CGImageSourceCopyPropertiesAtIndex(m_decoder, 0, imageSourceOptions());
    if (properties) {
        int w = 0, h = 0;
        CFNumberRef num = (CFNumberRef)CFDictionaryGetValue(properties, kCGImagePropertyPixelWidth);
        if (num)
            CFNumberGetValue(num, kCFNumberIntType, &w);
        num = (CFNumberRef)CFDictionaryGetValue(properties, kCGImagePropertyPixelHeight);
        if (num)
            CFNumberGetValue(num, kCFNumberIntType, &h);
        result = IntSize(w, h);            

        if (!m_isProgressive)
        {
            CFDictionaryRef jfifProperties = (CFDictionaryRef)CFDictionaryGetValue(properties, kCGImagePropertyJFIFDictionary);
            if (jfifProperties) {
                CFBooleanRef isProgCFBool = (CFBooleanRef)CFDictionaryGetValue(jfifProperties, kCGImagePropertyJFIFIsProgressive);
                if (isProgCFBool)
                    m_isProgressive = CFBooleanGetValue(isProgCFBool);
                // 5184655: Hang rendering very large progressive JPEG
                // Decoding progressive images hangs for a very long time right now
                // Until this is fixed, don't sub-sample progressive images
                // This will cause them to fail our large image check and they won't be decoded.
                // FIXME: remove once underlying issue is fixed (5191418)
            }
        }

        if ((m_baseSubsampling == 0) && !m_isProgressive) {
            while ((m_baseSubsampling < 3) && (result.width() * result.height() > 2000000)) {
                // Image is very large and should be sub-sampled.
                // Increase the base subsampling and ask for the size again. If the image can be subsampled, the size will be
                // greatly reduced. 4x sub-sampling will make us support up to 32MP images, which should be plenty.
                // There's no callback from ImageIO when the size is available, so we do the check when we happen
                // to check the size and its non - zero.
                // Note: some clients of this class don't call isSizeAvailable() so we can't rely on that.
                m_baseSubsampling++;
                result = size();
            }
        }    

        CFRelease(properties);
    }
    return result;
}

int ImageSource::repetitionCount()
{
    int result = cAnimationLoopOnce; // No property means loop once.
        
    // A property with value 0 means loop forever.
    CFDictionaryRef properties = CGImageSourceCopyProperties(m_decoder, imageSourceOptions());
    if (properties) {
        CFDictionaryRef gifProperties = (CFDictionaryRef)CFDictionaryGetValue(properties, kCGImagePropertyGIFDictionary);
        if (gifProperties) {
            CFNumberRef num = (CFNumberRef)CFDictionaryGetValue(gifProperties, kCGImagePropertyGIFLoopCount);
            if (num)
                CFNumberGetValue(num, kCFNumberIntType, &result);
        } else
            result = cAnimationNone; // Turns out we're not a GIF after all, so we don't animate.
        
        CFRelease(properties);
    }
    
    return result;
}

size_t ImageSource::frameCount() const
{
    return m_decoder ? CGImageSourceGetCount(m_decoder) : 0;
}

CGImageRef ImageSource::createFrameAtIndex(size_t index, float scaleHint, float* actualScaleOut, ssize_t* bytesOut)
{
    // subsampling can be 0, 1, 2 or 3, which means full-, quarter-, sixteenth- and sixty-fourth-size, respectively.
    int subsampling = (int)log2f(1.0f / std::max(0.1f, std::min(1.0f, scaleHint)));
    CGImageRef image = CGImageSourceCreateImageAtIndex(m_decoder, index, imageSourceOptions(subsampling));
    *actualScaleOut = float(CGImageGetWidth(image)) / float(size().width()); // height could be anything while downloading
    *bytesOut = CGImageGetBytesPerRow(image) * CGImageGetHeight(image);
    return image;
}

bool ImageSource::frameIsCompleteAtIndex(size_t index)
{
    return CGImageSourceGetStatusAtIndex(m_decoder, index) == kCGImageStatusComplete;
}

float ImageSource::frameDurationAtIndex(size_t index)
{
    float duration = 0;
    CFDictionaryRef properties = CGImageSourceCopyPropertiesAtIndex(m_decoder, index, imageSourceOptions());
    if (properties) {
        CFDictionaryRef typeProperties = (CFDictionaryRef)CFDictionaryGetValue(properties, kCGImagePropertyGIFDictionary);
        if (typeProperties) {
            CFNumberRef num = (CFNumberRef)CFDictionaryGetValue(typeProperties, kCGImagePropertyGIFDelayTime);
            if (num)
                CFNumberGetValue(num, kCFNumberFloatType, &duration);
        }
        CFRelease(properties);
    }

    // Many annoying ads specify a 0 duration to make an image flash as quickly as possible.
    // We follow WinIE's behavior and use a duration of 100 ms for any frames that specify
    // a duration of <= 50 ms. See <http://bugs.webkit.org/show_bug.cgi?id=14413> or Radar 4051389 for more.
    if (duration < 0.051f)
        return 0.100f;
    return duration;
}

bool ImageSource::frameHasAlphaAtIndex(size_t index)
{
    CFStringRef imageType = CGImageSourceGetType(m_decoder);

    // Return false for JPEG
    if (CFEqual(imageType, CFSTR("public.jpeg")))
        return false;

    // FIXME: Return false for other non-transparent image formats.
    // FIXME: Could maybe return false for a GIF Frame if we have enough info in the GIF properties dictionary
    // to determine whether or not a transparent color was defined.
    return true;
}

}

#endif // PLATFORM(CG)