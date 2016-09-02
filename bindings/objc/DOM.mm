/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2006 James G. Speth (speth@end.com)
 * Copyright (C) 2006 Samuel Weinig (sam.weinig@gmail.com)
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

#import "config.h"
#import "DOMInternal.h" // import first to make the private/public trick work
#import "DOM.h"

#import "DOMElementInternal.h"
#import "DOMHTMLCanvasElement.h"
#import "DOMNodeInternal.h"
#import "DOMPrivate.h"
#import "DOMRangeInternal.h"
#import "Frame.h"
#import "HTMLElement.h"
#import "HTMLNames.h"
#import "NodeFilter.h"
#import "RenderImage.h"
#import "WebScriptObjectPrivate.h"
#import <wtf/HashMap.h>

#if ENABLE(SVG_DOM_OBJC_BINDINGS)
#import "DOMSVG.h"
#import "SVGElementInstance.h"
#import "SVGNames.h"
#endif

#import "HTMLLinkElement.h"
#import "KeyboardEvent.h"
#import "KURL.h"
#import "MediaList.h"
#import "MediaQueryEvaluator.h"
#import "NodeRenderStyle.h"
#import "RenderView.h"
#import "Touch.h"
#import "WAKAppKitStubs.h"
#import "WAKWindow.h"
#import "WKWindowPrivate.h"
#import "WebCoreThreadMessage.h"

using namespace JSC;
using namespace WebCore;

// FIXME: Would be nice to break this up into separate files to match how other WebKit
// code is organized.

//------------------------------------------------------------------------------------------
// DOMNode

namespace WebCore {

typedef HashMap<const QualifiedName::QualifiedNameImpl*, Class> ObjCClassMap;
static ObjCClassMap* elementClassMap;

static void addElementClass(const QualifiedName& tag, Class objCClass)
{
    elementClassMap->set(tag.impl(), objCClass);
}

static void createElementClassMap()
{
    // Create the table.
    elementClassMap = new ObjCClassMap;

    // FIXME: Reflect marquee once the API has been determined.

    // Populate it with HTML and SVG element classes.
    addElementClass(HTMLNames::aTag, [DOMHTMLAnchorElement class]);
    addElementClass(HTMLNames::appletTag, [DOMHTMLAppletElement class]);
    addElementClass(HTMLNames::areaTag, [DOMHTMLAreaElement class]);
    addElementClass(HTMLNames::baseTag, [DOMHTMLBaseElement class]);
    addElementClass(HTMLNames::basefontTag, [DOMHTMLBaseFontElement class]);
    addElementClass(HTMLNames::bodyTag, [DOMHTMLBodyElement class]);
    addElementClass(HTMLNames::brTag, [DOMHTMLBRElement class]);
    addElementClass(HTMLNames::buttonTag, [DOMHTMLButtonElement class]);
    addElementClass(HTMLNames::canvasTag, [DOMHTMLCanvasElement class]);
    addElementClass(HTMLNames::captionTag, [DOMHTMLTableCaptionElement class]);
    addElementClass(HTMLNames::colTag, [DOMHTMLTableColElement class]);
    addElementClass(HTMLNames::colgroupTag, [DOMHTMLTableColElement class]);
    addElementClass(HTMLNames::delTag, [DOMHTMLModElement class]);
    addElementClass(HTMLNames::dirTag, [DOMHTMLDirectoryElement class]);
    addElementClass(HTMLNames::divTag, [DOMHTMLDivElement class]);
    addElementClass(HTMLNames::dlTag, [DOMHTMLDListElement class]);
    addElementClass(HTMLNames::embedTag, [DOMHTMLEmbedElement class]);
    addElementClass(HTMLNames::fieldsetTag, [DOMHTMLFieldSetElement class]);
    addElementClass(HTMLNames::fontTag, [DOMHTMLFontElement class]);
    addElementClass(HTMLNames::formTag, [DOMHTMLFormElement class]);
    addElementClass(HTMLNames::frameTag, [DOMHTMLFrameElement class]);
    addElementClass(HTMLNames::framesetTag, [DOMHTMLFrameSetElement class]);
    addElementClass(HTMLNames::h1Tag, [DOMHTMLHeadingElement class]);
    addElementClass(HTMLNames::h2Tag, [DOMHTMLHeadingElement class]);
    addElementClass(HTMLNames::h3Tag, [DOMHTMLHeadingElement class]);
    addElementClass(HTMLNames::h4Tag, [DOMHTMLHeadingElement class]);
    addElementClass(HTMLNames::h5Tag, [DOMHTMLHeadingElement class]);
    addElementClass(HTMLNames::h6Tag, [DOMHTMLHeadingElement class]);
    addElementClass(HTMLNames::headTag, [DOMHTMLHeadElement class]);
    addElementClass(HTMLNames::hrTag, [DOMHTMLHRElement class]);
    addElementClass(HTMLNames::htmlTag, [DOMHTMLHtmlElement class]);
    addElementClass(HTMLNames::iframeTag, [DOMHTMLIFrameElement class]);
    addElementClass(HTMLNames::imgTag, [DOMHTMLImageElement class]);
    addElementClass(HTMLNames::inputTag, [DOMHTMLInputElement class]);
    addElementClass(HTMLNames::insTag, [DOMHTMLModElement class]);
    addElementClass(HTMLNames::isindexTag, [DOMHTMLIsIndexElement class]);
    addElementClass(HTMLNames::labelTag, [DOMHTMLLabelElement class]);
    addElementClass(HTMLNames::legendTag, [DOMHTMLLegendElement class]);
    addElementClass(HTMLNames::liTag, [DOMHTMLLIElement class]);
    addElementClass(HTMLNames::linkTag, [DOMHTMLLinkElement class]);
    addElementClass(HTMLNames::listingTag, [DOMHTMLPreElement class]);
    addElementClass(HTMLNames::mapTag, [DOMHTMLMapElement class]);
    addElementClass(HTMLNames::marqueeTag, [DOMHTMLMarqueeElement class]);
    addElementClass(HTMLNames::menuTag, [DOMHTMLMenuElement class]);
    addElementClass(HTMLNames::metaTag, [DOMHTMLMetaElement class]);
    addElementClass(HTMLNames::objectTag, [DOMHTMLObjectElement class]);
    addElementClass(HTMLNames::olTag, [DOMHTMLOListElement class]);
    addElementClass(HTMLNames::optgroupTag, [DOMHTMLOptGroupElement class]);
    addElementClass(HTMLNames::optionTag, [DOMHTMLOptionElement class]);
    addElementClass(HTMLNames::pTag, [DOMHTMLParagraphElement class]);
    addElementClass(HTMLNames::paramTag, [DOMHTMLParamElement class]);
    addElementClass(HTMLNames::preTag, [DOMHTMLPreElement class]);
    addElementClass(HTMLNames::qTag, [DOMHTMLQuoteElement class]);
    addElementClass(HTMLNames::scriptTag, [DOMHTMLScriptElement class]);
    addElementClass(HTMLNames::keygenTag, [DOMHTMLSelectElement class]);
    addElementClass(HTMLNames::selectTag, [DOMHTMLSelectElement class]);
    addElementClass(HTMLNames::styleTag, [DOMHTMLStyleElement class]);
    addElementClass(HTMLNames::tableTag, [DOMHTMLTableElement class]);
    addElementClass(HTMLNames::tbodyTag, [DOMHTMLTableSectionElement class]);
    addElementClass(HTMLNames::tdTag, [DOMHTMLTableCellElement class]);
    addElementClass(HTMLNames::textareaTag, [DOMHTMLTextAreaElement class]);
    addElementClass(HTMLNames::tfootTag, [DOMHTMLTableSectionElement class]);
    addElementClass(HTMLNames::thTag, [DOMHTMLTableCellElement class]);
    addElementClass(HTMLNames::theadTag, [DOMHTMLTableSectionElement class]);
    addElementClass(HTMLNames::titleTag, [DOMHTMLTitleElement class]);
    addElementClass(HTMLNames::trTag, [DOMHTMLTableRowElement class]);
    addElementClass(HTMLNames::ulTag, [DOMHTMLUListElement class]);
    addElementClass(HTMLNames::xmpTag, [DOMHTMLPreElement class]);

#if ENABLE(SVG_DOM_OBJC_BINDINGS)
    addElementClass(SVGNames::aTag, [DOMSVGAElement class]);
    addElementClass(SVGNames::altGlyphTag, [DOMSVGAltGlyphElement class]);
#if ENABLE(SVG_ANIMATION)
    addElementClass(SVGNames::animateTag, [DOMSVGAnimateElement class]);
    addElementClass(SVGNames::animateColorTag, [DOMSVGAnimateColorElement class]);
    addElementClass(SVGNames::animateTransformTag, [DOMSVGAnimateTransformElement class]);
    addElementClass(SVGNames::setTag, [DOMSVGSetElement class]);
#endif
    addElementClass(SVGNames::circleTag, [DOMSVGCircleElement class]);
    addElementClass(SVGNames::clipPathTag, [DOMSVGClipPathElement class]);
    addElementClass(SVGNames::cursorTag, [DOMSVGCursorElement class]);
    addElementClass(SVGNames::defsTag, [DOMSVGDefsElement class]);
    addElementClass(SVGNames::descTag, [DOMSVGDescElement class]);
    addElementClass(SVGNames::ellipseTag, [DOMSVGEllipseElement class]);
#if ENABLE(FILTERS)
    addElementClass(SVGNames::feBlendTag, [DOMSVGFEBlendElement class]);
    addElementClass(SVGNames::feColorMatrixTag, [DOMSVGFEColorMatrixElement class]);
    addElementClass(SVGNames::feComponentTransferTag, [DOMSVGFEComponentTransferElement class]);
    addElementClass(SVGNames::feCompositeTag, [DOMSVGFECompositeElement class]);
    addElementClass(SVGNames::feDiffuseLightingTag, [DOMSVGFEDiffuseLightingElement class]);
    addElementClass(SVGNames::feDisplacementMapTag, [DOMSVGFEDisplacementMapElement class]);
    addElementClass(SVGNames::feDistantLightTag, [DOMSVGFEDistantLightElement class]);
    addElementClass(SVGNames::feFloodTag, [DOMSVGFEFloodElement class]);
    addElementClass(SVGNames::feFuncATag, [DOMSVGFEFuncAElement class]);
    addElementClass(SVGNames::feFuncBTag, [DOMSVGFEFuncBElement class]);
    addElementClass(SVGNames::feFuncGTag, [DOMSVGFEFuncGElement class]);
    addElementClass(SVGNames::feFuncRTag, [DOMSVGFEFuncRElement class]);
    addElementClass(SVGNames::feGaussianBlurTag, [DOMSVGFEGaussianBlurElement class]);
    addElementClass(SVGNames::feImageTag, [DOMSVGFEImageElement class]);
    addElementClass(SVGNames::feMergeTag, [DOMSVGFEMergeElement class]);
    addElementClass(SVGNames::feMergeNodeTag, [DOMSVGFEMergeNodeElement class]);
    addElementClass(SVGNames::feMorphologyTag, [DOMSVGFEMorphologyElement class]);
    addElementClass(SVGNames::feOffsetTag, [DOMSVGFEOffsetElement class]);
    addElementClass(SVGNames::fePointLightTag, [DOMSVGFEPointLightElement class]);
    addElementClass(SVGNames::feSpecularLightingTag, [DOMSVGFESpecularLightingElement class]);
    addElementClass(SVGNames::feSpotLightTag, [DOMSVGFESpotLightElement class]);
    addElementClass(SVGNames::feTileTag, [DOMSVGFETileElement class]);
    addElementClass(SVGNames::feTurbulenceTag, [DOMSVGFETurbulenceElement class]);
    addElementClass(SVGNames::filterTag, [DOMSVGFilterElement class]);
#endif
#if ENABLE(SVG_FONTS)
    addElementClass(SVGNames::fontTag, [DOMSVGFontElement class]);
    addElementClass(SVGNames::font_faceTag, [DOMSVGFontFaceElement class]);
    addElementClass(SVGNames::font_face_formatTag, [DOMSVGFontFaceFormatElement class]);
    addElementClass(SVGNames::font_face_nameTag, [DOMSVGFontFaceNameElement class]);
    addElementClass(SVGNames::font_face_srcTag, [DOMSVGFontFaceSrcElement class]);
    addElementClass(SVGNames::font_face_uriTag, [DOMSVGFontFaceUriElement class]);
    addElementClass(SVGNames::glyphTag, [DOMSVGGlyphElement class]);
#endif
    addElementClass(SVGNames::gTag, [DOMSVGGElement class]);
    addElementClass(SVGNames::imageTag, [DOMSVGImageElement class]);
    addElementClass(SVGNames::lineTag, [DOMSVGLineElement class]);
    addElementClass(SVGNames::linearGradientTag, [DOMSVGLinearGradientElement class]);
    addElementClass(SVGNames::markerTag, [DOMSVGMarkerElement class]);
    addElementClass(SVGNames::maskTag, [DOMSVGMaskElement class]);
    addElementClass(SVGNames::metadataTag, [DOMSVGMetadataElement class]);
#if ENABLE(SVG_FONTS)
    addElementClass(SVGNames::missing_glyphTag, [DOMSVGMissingGlyphElement class]);
#endif
    addElementClass(SVGNames::pathTag, [DOMSVGPathElement class]);
    addElementClass(SVGNames::patternTag, [DOMSVGPatternElement class]);
    addElementClass(SVGNames::polygonTag, [DOMSVGPolygonElement class]);
    addElementClass(SVGNames::polylineTag, [DOMSVGPolylineElement class]);
    addElementClass(SVGNames::radialGradientTag, [DOMSVGRadialGradientElement class]);
    addElementClass(SVGNames::rectTag, [DOMSVGRectElement class]);
    addElementClass(SVGNames::scriptTag, [DOMSVGScriptElement class]);
    addElementClass(SVGNames::stopTag, [DOMSVGStopElement class]);
    addElementClass(SVGNames::styleTag, [DOMSVGStyleElement class]);
    addElementClass(SVGNames::svgTag, [DOMSVGSVGElement class]);
    addElementClass(SVGNames::switchTag, [DOMSVGSwitchElement class]);
    addElementClass(SVGNames::symbolTag, [DOMSVGSymbolElement class]);
    addElementClass(SVGNames::textTag, [DOMSVGTextElement class]);
    addElementClass(SVGNames::titleTag, [DOMSVGTitleElement class]);
    addElementClass(SVGNames::trefTag, [DOMSVGTRefElement class]);
    addElementClass(SVGNames::tspanTag, [DOMSVGTSpanElement class]);
    addElementClass(SVGNames::textPathTag, [DOMSVGTextPathElement class]);
    addElementClass(SVGNames::useTag, [DOMSVGUseElement class]);
    addElementClass(SVGNames::viewTag, [DOMSVGViewElement class]);
#endif
}

static Class lookupElementClass(const QualifiedName& tag)
{
    // Do a special lookup to ignore element prefixes
    if (tag.hasPrefix())
        return elementClassMap->get(QualifiedName(nullAtom, tag.localName(), tag.namespaceURI()).impl());
    
    return elementClassMap->get(tag.impl());
}

static Class elementClass(const QualifiedName& tag, Class defaultClass)
{
    if (!elementClassMap)
        createElementClassMap();
    Class objcClass = lookupElementClass(tag);
    if (!objcClass)
        objcClass = defaultClass;
    return objcClass;
}

static NSArray *kit(const Vector<IntRect>& rects)
{
    size_t size = rects.size();
    NSMutableArray *array = [NSMutableArray arrayWithCapacity:size];
    for (size_t i = 0; i < size; ++i)
        [array addObject:[NSValue valueWithRect:rects[i]]];
    return array;
}

static WKQuad wkQuadFromFloatQuad(const FloatQuad& inQuad)
{
    WKQuad  theQuad;
    theQuad.p1 = inQuad.p1();
    theQuad.p2 = inQuad.p2();
    theQuad.p3 = inQuad.p3();
    theQuad.p4 = inQuad.p4();
    
    return theQuad;
}

static NSArray *kit(const Vector<FloatQuad>& quads)
{
    size_t size = quads.size();
    NSMutableArray *array = [NSMutableArray arrayWithCapacity:size];
    for (size_t i = 0; i < size; ++i) {
        WKQuadObject* quadObject = [[WKQuadObject alloc] initWithQuad:wkQuadFromFloatQuad(quads[i])];
        [array addObject:quadObject];
        [quadObject release];
    }
    return array;
}

static inline float min4(float a, float b, float c, float d)
{
    return std::min(std::min(a, b), std::min(c, d));
}

static inline float max4(float a, float b, float c, float d)
{
    return std::max(std::max(a, b), std::max(c, d));
}

static inline WKQuad emptyQuad()
{
    WKQuad zeroQuad = { CGPointZero, CGPointZero, CGPointZero, CGPointZero };
    return zeroQuad;
}

} // namespace WebCore

@implementation WKQuadObject

- (id)initWithQuad:(WKQuad)quad
{
    if ((self = [super init]))
    {
        _quad = quad;
    }
    return self;
}

- (WKQuad)quad
{
    return _quad;
}

- (CGRect)boundingBox
{
    float left      = WebCore::min4(_quad.p1.x, _quad.p2.x, _quad.p3.x, _quad.p4.x);
    float top       = WebCore::min4(_quad.p1.y, _quad.p2.y, _quad.p3.y, _quad.p4.y);
    
    float right     = WebCore::max4(_quad.p1.x, _quad.p2.x, _quad.p3.x, _quad.p4.x);
    float bottom    = WebCore::max4(_quad.p1.y, _quad.p2.y, _quad.p3.y, _quad.p4.y);
    
    return CGRectMake(left, top, right - left, bottom - top);
}

@end

@implementation DOMNode (WebCoreInternal)

- (NSString *)description
{
    if (!_internal)
        return [NSString stringWithFormat:@"<%@: null>", [[self class] description], self];

    NSString *value = [self nodeValue];
    if (value)
        return [NSString stringWithFormat:@"<%@ [%@]: %p '%@'>",
            [[self class] description], [self nodeName], _internal, value];

    return [NSString stringWithFormat:@"<%@ [%@]: %p>", [[self class] description], [self nodeName], _internal];
}

- (JSC::Bindings::RootObject*)_rootObject
{
    WebCore::Frame* frame = core(self)->document()->frame();
    if (!frame)
        return 0;
    return frame->script()->bindingRootObject();
}

@end

Class kitClass(WebCore::Node* impl)
{
    switch (impl->nodeType()) {
        case WebCore::Node::ELEMENT_NODE:
            if (impl->isHTMLElement())
                return WebCore::elementClass(static_cast<WebCore::HTMLElement*>(impl)->tagQName(), [DOMHTMLElement class]);
#if ENABLE(SVG_DOM_OBJC_BINDINGS)
            if (impl->isSVGElement())
                return WebCore::elementClass(static_cast<WebCore::SVGElement*>(impl)->tagQName(), [DOMSVGElement class]);
#endif
            return [DOMElement class];
        case WebCore::Node::ATTRIBUTE_NODE:
            return [DOMAttr class];
        case WebCore::Node::TEXT_NODE:
            return [DOMText class];
        case WebCore::Node::CDATA_SECTION_NODE:
            return [DOMCDATASection class];
        case WebCore::Node::ENTITY_REFERENCE_NODE:
            return [DOMEntityReference class];
        case WebCore::Node::ENTITY_NODE:
            return [DOMEntity class];
        case WebCore::Node::PROCESSING_INSTRUCTION_NODE:
            return [DOMProcessingInstruction class];
        case WebCore::Node::COMMENT_NODE:
            return [DOMComment class];
        case WebCore::Node::DOCUMENT_NODE:
            if (static_cast<WebCore::Document*>(impl)->isHTMLDocument())
                return [DOMHTMLDocument class];
#if ENABLE(SVG_DOM_OBJC_BINDINGS)
            if (static_cast<WebCore::Document*>(impl)->isSVGDocument())
                return [DOMSVGDocument class];
#endif
            return [DOMDocument class];
        case WebCore::Node::DOCUMENT_TYPE_NODE:
            return [DOMDocumentType class];
        case WebCore::Node::DOCUMENT_FRAGMENT_NODE:
            return [DOMDocumentFragment class];
        case WebCore::Node::NOTATION_NODE:
            return [DOMNotation class];
        case WebCore::Node::XPATH_NAMESPACE_NODE:
            // FIXME: Create an XPath objective C wrapper
            // See http://bugs.webkit.org/show_bug.cgi?id=8755
            return nil;
    }
    ASSERT_NOT_REACHED();
    return nil;
}

id <DOMEventTarget> kit(WebCore::EventTarget* eventTarget)
{
    if (!eventTarget)
        return nil;

    if (WebCore::Node* node = eventTarget->toNode())
        return kit(node);

#if ENABLE(SVG_DOM_OBJC_BINDINGS)
    if (WebCore::SVGElementInstance* svgElementInstance = eventTarget->toSVGElementInstance())
        return kit(svgElementInstance);
#endif

    // We don't have an ObjC binding for XMLHttpRequest.

    return nil;
}

@implementation DOMNode (DOMNodeExtensions)

- (CGRect)boundingBox
{
    // FIXME: Could we move this function to WebCore::Node and autogenerate?
    core(self)->document()->updateLayoutIgnorePendingStylesheets();
    WebCore::RenderObject* renderer = core(self)->renderer();
    if (!renderer)
        return CGRectZero;
    return renderer->absoluteBoundingBoxRect();
}

- (NSArray *)lineBoxRects
{
    return [self textRects];
}

// quad in page coordinates, taking transforms into account. c.f. - (NSRect)boundingBox;
- (WKQuad)absoluteQuad
{
    core(self)->document()->updateLayoutIgnorePendingStylesheets();
    WebCore::RenderObject *renderer = core(self)->renderer();
    if (renderer) {
        Vector<FloatQuad> quads;
        renderer->absoluteQuads(quads);
        if (quads.size() == 0)
            return WebCore::emptyQuad();
        
        if (quads.size() == 1)
            return wkQuadFromFloatQuad(quads[0]);

        FloatRect boundingRect = quads[0].boundingBox();
        for (size_t i = 1; i < quads.size(); ++i)
            boundingRect.unite(quads[i].boundingBox());

        return wkQuadFromFloatQuad(boundingRect);
    }

    return WebCore::emptyQuad();
}

// returns array of WKQuadObject
- (NSArray *)lineBoxQuads
{
    core(self)->document()->updateLayoutIgnorePendingStylesheets();
    WebCore::RenderObject *renderer = core(self)->renderer();
    if (renderer) {
        Vector<WebCore::FloatQuad> quads;
        renderer->absoluteQuads(quads);
        return kit(quads);
    }
    return nil;
}

- (Element *)_linkElement
{
    WebCore::Node* node = core(self);
    
    while (node) {
        if (node->isLink())
            return static_cast<WebCore::Element*>(node);
        node = node->parentNode();
    }
    
    return 0;
}

- (NSURL *)hrefURL
{
    Element *link= [self _linkElement];
    if (link)
        return link->document()->completeURL(deprecatedParseURL(link->getAttribute("href")));
    
    return nil;
}

- (NSString *)hrefTarget
{
    Element *target = [self _linkElement];
    
    if(target) return target->getAttribute("target");
    
    return nil;
}

- (CGRect)hrefFrame
{
    RenderObject *renderer = [self _linkElement]->renderer();
    
    if(renderer) return renderer->absoluteBoundingBoxRect();
    
    return NSZeroRect;
}

- (NSString *)hrefLabel
{
    Element *link= [self _linkElement];
    
    if (!link) return nil;
    
    return link->textContent();
}

- (NSString *)hrefTitle
{
    Element *link= [self _linkElement];
    
    if (!link) return nil;
    
    return link->document()->displayStringModifiedByEncoding(static_cast<HTMLElement *>(link)->title());
}

- (CGRect)boundingFrame
{
    return [self boundingBox];
}

- (CGRect)innerFrame
{
	WebCore::Node * node = core(self);
	RenderObject * renderer = node->renderer();
	
	if (!renderer) return CGRectZero;
    
	RenderStyle * style = renderer->style();
	CGRect innerFrame = [self boundingFrame];
	
	innerFrame.origin.x += style->borderLeftWidth();
	innerFrame.size.width -= style->borderLeftWidth() + style->borderRightWidth();
    
	innerFrame.origin.y += style->borderBottomWidth();  // not top?
	innerFrame.size.height -= style->borderBottomWidth() + style->borderTopWidth();
	
	return innerFrame;
}

- (WKQuad)innerFrameQuad       // takes transforms into account
{
    core(self)->document()->updateLayoutIgnorePendingStylesheets();
    WebCore::RenderObject *renderer = core(self)->renderer();
    if (!renderer)
        return emptyQuad();

	RenderStyle * style = renderer->style();
    IntRect boundingBox = renderer->absoluteBoundingBoxRect(true /* use transforms*/);

	boundingBox.move(style->borderLeftWidth(), style->borderBottomWidth());  // not top?
	boundingBox.setWidth(boundingBox.width() - style->borderLeftWidth() + style->borderRightWidth());
	boundingBox.setHeight(boundingBox.height() - style->borderBottomWidth() + style->borderTopWidth());

    return wkQuadFromFloatQuad(FloatQuad(boundingBox));
}

- (float)computedFontSize
{
    WebCore::Node *node = core(self);
    RenderStyle *style = node->renderStyle();
    if (!style)
        return 0.0f;
    return style->fontDescription().computedSize();
}

- (DOMNode *)nextFocusNode
{
    RefPtr<KeyboardEvent> key = KeyboardEvent::create();
    return kit(core(self)->document()->nextFocusableNode(core(self), key.get()));
}

- (DOMNode *)previousFocusNode
{
    RefPtr<KeyboardEvent> key = KeyboardEvent::create();
    return kit(core(self)->document()->previousFocusableNode(core(self), key.get()));
}


@end

@implementation DOMNode (DOMNodeExtensionsPendingPublic)


- (NSArray *)textRects
{
    // FIXME: Could we move this function to WebCore::Node and autogenerate?
    core(self)->document()->updateLayoutIgnorePendingStylesheets();
    if (!core(self)->renderer())
        return nil;
    RefPtr<Range> range = Range::create(core(self)->document());
    WebCore::ExceptionCode ec = 0;
    range->selectNodeContents(core(self), ec);
    Vector<WebCore::IntRect> rects;
    range->textRects(rects);
    return kit(rects);
}
@end

@implementation DOMRange (DOMRangeExtensions)

- (CGRect)boundingBox
{
    // FIXME: The call to updateLayoutIgnorePendingStylesheets should be moved into WebCore::Range.
    core(self)->ownerDocument()->updateLayoutIgnorePendingStylesheets();
    return core(self)->boundingBox();
}

- (NSArray *)textRects
{
    // FIXME: The call to updateLayoutIgnorePendingStylesheets should be moved into WebCore::Range.
    Vector<WebCore::IntRect> rects;
    core(self)->ownerDocument()->updateLayoutIgnorePendingStylesheets();
    core(self)->textRects(rects);
    return kit(rects);
}

- (NSArray *)lineBoxRects
{
    // FIXME: Remove this once all clients stop using it and we drop Leopard support.
    return [self textRects];
}

@end

//------------------------------------------------------------------------------------------
// DOMElement

@implementation DOMElement (DOMElementAppKitExtensions)


@end

@implementation DOMElement (WebPrivate)

- (GSFontRef)_font
{
    RenderObject* renderer = core(self)->renderer();
    if (!renderer)
        return nil;
    return renderer->style()->font().primaryFont()->getGSFont();
}


- (NSURL *)_getURLAttribute:(NSString *)name
{
    // FIXME: Could we move this function to WebCore::Element and autogenerate?
    ASSERT(name);
    WebCore::Element* element = core(self);
    ASSERT(element);
    return element->document()->completeURL(deprecatedParseURL(element->getAttribute(name)));
}

- (BOOL)isFocused
{
    // FIXME: Could we move this function to WebCore::Element and autogenerate?
    WebCore::Element* element = core(self);
    return element->document()->focusedNode() == element;
}

@end

@implementation DOMHTMLLinkElement (WebPrivate)
- (BOOL)_mediaQueryMatchesForOrientation:(int)orientation
{
    HTMLLinkElement* link = static_cast<HTMLLinkElement*>(core(self));
    String media = link->media();
    if (media.isEmpty())
        return true;
    Document* document = link->document();
    FrameView* frameView = document->frame() ? document->frame()->view() : 0;
    if (!frameView)
        return false;
    int layoutWidth = frameView->layoutWidth();
    int layoutHeight = frameView->layoutHeight();
    IntSize savedFixedLayoutSize = frameView->fixedLayoutSize();
    bool savedUseFixedLayout = frameView->useFixedLayout();
    if ((orientation == WebMediaQueryOrientationPortrait && layoutWidth > layoutHeight) ||
        (orientation == WebMediaQueryOrientationLandscape && layoutWidth < layoutHeight)) {
        // temporarily swap the orientation for the evaluation
        frameView->setFixedLayoutSize(IntSize(layoutHeight, layoutWidth));
        frameView->setUseFixedLayout(true);
    }
    
    RefPtr<MediaList> mediaList = MediaList::createAllowingDescriptionSyntax(media);
    MediaQueryEvaluator screenEval("screen", document->frame(), document->renderer() ? document->renderer()->style() : 0);
    
    bool result = screenEval.eval(mediaList.get());

    frameView->setFixedLayoutSize(savedFixedLayoutSize);
    frameView->setUseFixedLayout(savedUseFixedLayout);

    return result;
}
@end

//------------------------------------------------------------------------------------------
// DOMRange

@implementation DOMRange (WebPrivate)

- (NSString *)description
{
    if (!_internal)
        return @"<DOMRange: null>";
    return [NSString stringWithFormat:@"<DOMRange: %@ %d %@ %d>",
               [self startContainer], [self startOffset], [self endContainer], [self endOffset]];
}

// FIXME: This should be removed as soon as all internal Apple uses of it have been replaced with
// calls to the public method - (NSString *)text.
- (NSString *)_text
{
    return [self text];
}

@end

//------------------------------------------------------------------------------------------
// DOMRGBColor

@implementation DOMRGBColor (WebPrivate)


@end


//------------------------------------------------------------------------------------------
// DOMNodeFilter

DOMNodeFilter *kit(WebCore::NodeFilter* impl)
{
    if (!impl)
        return nil;
    
    if (DOMNodeFilter *wrapper = getDOMWrapper(impl))
        return [[wrapper retain] autorelease];
    
    DOMNodeFilter *wrapper = [[DOMNodeFilter alloc] _init];
    wrapper->_internal = reinterpret_cast<DOMObjectInternal*>(impl);
    impl->ref();
    addDOMWrapper(wrapper, impl);
    return [wrapper autorelease];
}

WebCore::NodeFilter* core(DOMNodeFilter *wrapper)
{
    return wrapper ? reinterpret_cast<WebCore::NodeFilter*>(wrapper->_internal) : 0;
}

@implementation DOMNodeFilter

- (void)dealloc
{
    if (_internal)
        reinterpret_cast<WebCore::NodeFilter*>(_internal)->deref();
    [super dealloc];
}

- (void)finalize
{
    if (_internal)
        reinterpret_cast<WebCore::NodeFilter*>(_internal)->deref();
    [super finalize];
}

- (short)acceptNode:(DOMNode *)node
{
    return core(self)->acceptNode(core(node));
}

@end