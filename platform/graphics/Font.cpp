/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2003, 2006 Apple Inc. All rights reserved.
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

#include "config.h"
#include "Font.h"

#include "FloatRect.h"
#include "FontCache.h"
#include "FontFallbackList.h"
#include "IntPoint.h"
#include "GlyphBuffer.h"
#include "WidthIterator.h"
#include <wtf/MathExtras.h>
#include <wtf/UnusedParam.h>

using namespace WTF;
using namespace Unicode;

namespace WebCore {

#if USE(FONT_FAST_PATH)
const uint8_t Font::gRoundingHackCharacterTable[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1 /*\t*/, 1 /*\n*/, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1 /*space*/, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 /*-*/, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 /*?*/,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1 /*no-break space*/, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

Font::CodePath Font::s_codePath = Auto;
#endif

// ============================================================================================
// Font Implementation (Cross-Platform Portion)
// ============================================================================================

Font::Font()
    : m_letterSpacing(0)
    , m_wordSpacing(0)
    , m_isPlatformFont(false)
{
}

Font::Font(const FontDescription& fd, float letterSpacing, float wordSpacing) 
    : m_fontDescription(fd)
    , m_letterSpacing(letterSpacing)
    , m_wordSpacing(wordSpacing)
    , m_isPlatformFont(false)
{
}

Font::Font(const FontPlatformData& fontData, bool isPrinterFont)
    : m_fontList(FontFallbackList::create())
    , m_letterSpacing(0)
    , m_wordSpacing(0)
    , m_isPlatformFont(true)
{
    m_fontDescription.setUsePrinterFont(isPrinterFont);
    m_fontList->setPlatformFont(fontData);
    m_fontDescription.setSpecifiedSize (GSFontGetSize(fontData.font()));
    m_fontDescription.setComputedSize (GSFontGetSize(fontData.font()));
    m_fontDescription.setItalic (GSFontGetTraits(fontData.font()) & GSItalicFontMask);
    m_fontDescription.setWeight ((GSFontGetTraits(fontData.font()) & GSBoldFontMask) ? FontWeightBold : FontWeightNormal);
}

Font::Font(const Font& other)
    : m_fontDescription(other.m_fontDescription)
    , m_fontList(other.m_fontList)
    , m_letterSpacing(other.m_letterSpacing)
    , m_wordSpacing(other.m_wordSpacing)
    , m_isPlatformFont(other.m_isPlatformFont)
{
}

Font& Font::operator=(const Font& other)
{
    m_fontDescription = other.m_fontDescription;
    m_fontList = other.m_fontList;
    m_letterSpacing = other.m_letterSpacing;
    m_wordSpacing = other.m_wordSpacing;
    m_isPlatformFont = other.m_isPlatformFont;
    return *this;
}

Font::~Font()
{
}

bool Font::operator==(const Font& other) const
{
    // Our FontData don't have to be checked, since checking the font description will be fine.
    // FIXME: This does not work if the font was made with the FontPlatformData constructor.
    if ((m_fontList && m_fontList->loadingCustomFonts()) ||
        (other.m_fontList && other.m_fontList->loadingCustomFonts()))
        return false;
    
    FontSelector* first = m_fontList ? m_fontList->fontSelector() : 0;
    FontSelector* second = other.m_fontList ? other.m_fontList->fontSelector() : 0;
    
    return first == second
           && m_fontDescription == other.m_fontDescription
           && m_letterSpacing == other.m_letterSpacing
           && m_wordSpacing == other.m_wordSpacing
           && (m_fontList ? m_fontList->generation() : 0) == (other.m_fontList ? other.m_fontList->generation() : 0);
}

const SimpleFontData* Font::primaryFont() const
{
    ASSERT(m_fontList);
    return m_fontList->primarySimpleFontData(this);
}

const FontData* Font::fontDataAt(unsigned index) const
{
    ASSERT(m_fontList);
    return m_fontList->fontDataAt(this, index);
}

const FontData* Font::fontDataForCharacters(const UChar* characters, int length) const
{
    ASSERT(m_fontList);
    return m_fontList->fontDataForCharacters(this, characters, length);
}

void Font::update(PassRefPtr<FontSelector> fontSelector) const
{
    // FIXME: It is pretty crazy that we are willing to just poke into a RefPtr, but it ends up 
    // being reasonably safe (because inherited fonts in the render tree pick up the new
    // style anyway. Other copies are transient, e.g., the state in the GraphicsContext, and
    // won't stick around long enough to get you in trouble). Still, this is pretty disgusting,
    // and could eventually be rectified by using RefPtrs for Fonts themselves.
    if (!m_fontList)
        m_fontList = FontFallbackList::create();
    m_fontList->invalidate(fontSelector);
}

bool Font::isFixedPitch() const
{
    ASSERT(m_fontList);
    return m_fontList->isFixedPitch(this);
}

float Font::drawText(GraphicsContext* context, const TextRun& run, const FloatPoint& point, int from, int to) const
{
    // Don't draw anything while we are using custom fonts that are in the process of loading.
    if (m_fontList && m_fontList->loadingCustomFonts())
        return 0.0f;
    
    to = (to == -1 ? run.length() : to);

#if ENABLE(SVG_FONTS)
    if (primaryFont()->isSVGFont()) {
        drawTextUsingSVGFont(context, run, point, from, to);
        return 0.0f;
    }
#endif

#if USE(FONT_FAST_PATH)
    if (codePath(run) != Complex)
        return drawSimpleText(context, run, point, from, to);
#endif

    return drawComplexText(context, run, point, from, to);
}

float Font::floatWidth(const TextRun& run, HashSet<const SimpleFontData*>* fallbackFonts, GlyphOverflow* glyphOverflow) const
{
#if ENABLE(SVG_FONTS)
    if (primaryFont()->isSVGFont())
        return floatWidthUsingSVGFont(run);
#endif

#if USE(FONT_FAST_PATH)
    CodePath codePathToUse = codePath(run);
    if (codePathToUse != Complex) {
        // If the complex text implementation cannot return fallback fonts, avoid
        // returning them for simple text as well.
        static bool returnFallbackFonts = canReturnFallbackFontsForComplexText();
        return floatWidthForSimpleText(run, 0, returnFallbackFonts ? fallbackFonts : 0, codePathToUse == SimpleWithGlyphOverflow ? glyphOverflow : 0);
    }
#endif

    return floatWidthForComplexText(run, fallbackFonts, glyphOverflow);
}

float Font::floatWidth(const TextRun& run, int extraCharsAvailable, int& charsConsumed, String& glyphName) const
{
#if !ENABLE(SVG_FONTS)
    UNUSED_PARAM(extraCharsAvailable);
#else
    if (primaryFont()->isSVGFont())
        return floatWidthUsingSVGFont(run, extraCharsAvailable, charsConsumed, glyphName);
#endif

    charsConsumed = run.length();
    glyphName = "";

#if USE(FONT_FAST_PATH)
    if (codePath(run) != Complex)
        return floatWidthForSimpleText(run, 0);
#endif

    return floatWidthForComplexText(run);
}

FloatRect Font::selectionRectForText(const TextRun& run, const IntPoint& point, int h, int from, int to) const
{
#if ENABLE(SVG_FONTS)
    if (primaryFont()->isSVGFont())
        return selectionRectForTextUsingSVGFont(run, point, h, from, to);
#endif

    to = (to == -1 ? run.length() : to);

#if USE(FONT_FAST_PATH)
    if (codePath(run) != Complex)
        return selectionRectForSimpleText(run, point, h, from, to);
#endif

    return selectionRectForComplexText(run, point, h, from, to);
}

int Font::offsetForPosition(const TextRun& run, int x, bool includePartialGlyphs) const
{
#if ENABLE(SVG_FONTS)
    if (primaryFont()->isSVGFont())
        return offsetForPositionForTextUsingSVGFont(run, x, includePartialGlyphs);
#endif

#if USE(FONT_FAST_PATH)
    if (codePath(run) != Complex)
        return offsetForPositionForSimpleText(run, x, includePartialGlyphs);
#endif

    return offsetForPositionForComplexText(run, x, includePartialGlyphs);
}

#if ENABLE(SVG_FONTS)
bool Font::isSVGFont() const
{ 
    return primaryFont()->isSVGFont(); 
}
#endif

FontSelector* Font::fontSelector() const
{
    return m_fontList ? m_fontList->fontSelector() : 0;
}

String Font::normalizeSpaces(const String& string)
{
    unsigned length = string.length();
    Vector<UChar, 256> buffer(length);
    bool didReplacement = false;

    for (unsigned i = 0; i < length; ++i) {
        UChar originalCharacter = string[i];
        buffer[i] = normalizeSpaces(originalCharacter);
        if (buffer[i] != originalCharacter)
            didReplacement = true;
    }

    return didReplacement ? String(buffer.data(), length) : string;
}

static bool shouldUseFontSmoothing = true;

void Font::setShouldUseSmoothing(bool shouldUseSmoothing)
{
    ASSERT(isMainThread() || pthread_main_np());
    shouldUseFontSmoothing = shouldUseSmoothing;
}

bool Font::shouldUseSmoothing()
{
    return shouldUseFontSmoothing;
}

static CGFontSmoothingStyle fontSmoothingStyle = kCGFontSmoothingStyleMedium;

void Font::setSmoothingStyle(CGFontSmoothingStyle newStyle)
{
    fontSmoothingStyle = newStyle;
}

CGFontSmoothingStyle Font::smoothingStyle()
{
    return fontSmoothingStyle;
}

static CGFontAntialiasingStyle fontAntialiasingStyle = kCGFontAntialiasingStyleUnfiltered;

void Font::setAntialiasingStyle(CGFontAntialiasingStyle newStyle)
{
    fontAntialiasingStyle = newStyle;
}

CGFontAntialiasingStyle Font::antialiasingStyle()
{
    return fontAntialiasingStyle;
}

}
