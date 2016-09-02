/*
 * TileGridTile.mm
 * WebCore
 *
 * Copyright (C) 2011, Apple Inc.  All rights reserved.
 *
 * No license or rights are granted by Apple expressly or by implication,
 * estoppel, or otherwise, to Apple copyrights, patents, trademarks, trade
 * secrets or other rights.
 */

#include "config.h"
#include "TileGrid.h"

#if PLATFORM(IOS)

#include "Color.h"
#include "TileCache.h"
#include "TileGridTile.h"
#include "TileLayer.h"
#include "TileLayerPool.h"
#include "WAKWindow.h"
#include "WKGraphics.h"
#include <algorithm>
#include <functional>
#include <QuartzCore/QuartzCore.h>
#include <QuartzCore/QuartzCorePrivate.h>

namespace WebCore {

#if LOG_TILING
static int totalTileCount;
#endif

TileGridTile::TileGridTile(TileGrid* tileGrid, const IntRect& tileRect)
    : m_tileGrid(tileGrid)
    , m_rect(tileRect)
{
    ASSERT(!tileRect.isEmpty());
    IntSize pixelSize(m_rect.size());
    const CGFloat screenScale = m_tileGrid->tileCache()->screenScale();
    pixelSize.scale(screenScale);
    m_tileLayer = TileLayerPool::sharedPool()->takeLayerWithSize(pixelSize);
    if (!m_tileLayer) {
#if LOG_TILING
        NSLog(@"unable to reuse layer with size %d x %d, creating one", pixelSize.width(), pixelSize.height());
#endif
        m_tileLayer = adoptNS([[TileLayer alloc] init]);
    }
    TileLayer* layer = m_tileLayer.get();
    [layer setTileGrid:tileGrid];
    [layer setOpaque:m_tileGrid->tileCache()->tilesOpaque()];
    [layer setEdgeAntialiasingMask:0];
    [layer setNeedsLayoutOnGeometryChange:NO];
    [layer setContentsScale:screenScale];
    [layer setAcceleratesDrawing:m_tileGrid->tileCache()->acceleratedDrawingEnabled()];
    
    // Host layer may have other sublayers. Keep the tile layers at the beginning of the array
    // so they are painted behind everything else.
    [tileGrid->tileHostLayer() insertSublayer:layer atIndex:tileGrid->tileCount()];
    [layer setFrame:m_rect];
    invalidateRect(m_rect);
    showBorder(m_tileGrid->tileCache()->tileBordersVisible());
    
#if LOG_TILING
    ++totalTileCount;
    NSLog(@"new Tile (%d,%d) %d %d, count %d", tileRect.x(), tileRect.y(), tileRect.width(), tileRect.height(), totalTileCount);
#endif
}

TileGridTile::~TileGridTile() 
{
    [tileLayer() setTileGrid:0];
    [tileLayer() removeFromSuperlayer];
    TileLayerPool::sharedPool()->addLayer(tileLayer());
#if LOG_TILING
    --totalTileCount;
    NSLog(@"delete Tile (%d,%d) %d %d, count %d", m_rect.x(), m_rect.y(), m_rect.width(), m_rect.height(), totalTileCount);
#endif
}

void TileGridTile::invalidateRect(const IntRect& windowDirtyRect)
{
    IntRect dirtyRect = intersection(windowDirtyRect, m_rect);
    if (dirtyRect.isEmpty())
        return;
    dirtyRect.move(IntPoint() - m_rect.location());
    [tileLayer() setNeedsDisplayInRect:dirtyRect];
    
    if (m_tileGrid->tileCache()->tilePaintCountersVisible())
        [tileLayer() setNeedsDisplayInRect:CGRectMake(0, 0, 46, 25)];
}

void TileGridTile::setRect(const IntRect& tileRect)
{
    if (m_rect == tileRect)
        return;
    m_rect = tileRect;
    TileLayer* layer = m_tileLayer.get();
    [layer setFrame:m_rect];
    [layer setNeedsDisplay];
}

void TileGridTile::showBorder(bool flag)
{
    TileLayer* layer = m_tileLayer.get();
    if (flag) {
        [layer setBorderColor:cachedCGColor(m_tileGrid->tileCache()->colorForGridTileBorder(m_tileGrid), ColorSpaceDeviceRGB)];
        [layer setBorderWidth:0.5f];
    } else {
        [layer setBorderColor:nil];
        [layer setBorderWidth:0];
    }
}
    
}

#endif
