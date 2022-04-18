#global public
using Core, Pixel

struct ImageRect
{
    using frameWnd: Wnd
    movie:          Movie
    textureFrame:   Texture
}

impl IWnd for ImageRect
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        if !movie.isValid()
            return

        // Update movie, and update resulting texture frame if necessary
        if movie.update(getApp().timing.dt)
        {
            if movie.source != .SpriteSheet
            {
                getApp().renderer.updateTexture(textureFrame, movie.frame.pixels)
            }
        }

        painter := evt.bc.painter
        theme   := getTheme()
        rect    := getClientRectPadding()

        switch movie.source
        {
        case SpriteSheet:
            icon := Icon.from(&movie.imgList, movie.frameIndex, rect.width, rect.height)
            icon.paint(painter, rect.x, rect.y, getThemeColors().imageRect_Fg)
            invalidate()
        case Gif:
            painter.drawTexture(rect, textureFrame, getThemeColors().imageRect_Fg)
            invalidate()
        }
    }
}

impl ImageRect
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = @{}, id: WndId = null)->*ImageRect
    {
        Debug.assert(parent != null)
        res := Wnd.create'ImageRect(parent, null, position, id)
        return res
    }

    mtd setMovie(image: ImageList)
    {
        movie.set(image)
        invalidate()
    }

    mtd setMovie(fileName: string) throw
    {
        try movie.set(fileName)
        textureFrame = getApp().renderer.addImage(movie.frame)
        invalidate()
    }
}