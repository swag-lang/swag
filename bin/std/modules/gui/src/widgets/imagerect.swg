#global public
using Core, Pixel

struct ImageRect
{
    using wnd:        Wnd
    movie:            Movie
    textureFrame:     Texture
}

impl IWnd for ImageRect
{
    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        if !me.movie.isValid():
            return

        // Update movie, and update resulting texture frame if necessary
        if me.movie.update(me.getApp().timing.dt)
        {
            if me.movie.source != .SpriteSheet
            {
                me.getApp().renderer.updateTexture(me.textureFrame, me.movie.frame.pixels.buffer)
            }
        }

        let painter = evt.bc.painter
        let rect    = me.getClientRectPadding()

        let color = me.getThemeColors().imageRect_Fg

        #[Swag.Complete]
        switch me.movie.source
        {
        case None:
            break
        case SpriteSheet:
            let icon = Icon.from(&me.movie.imgList, me.movie.frameIndex, rect.width, rect.height)
            icon.paint(painter, rect.x, rect.y, color)
        case Gif:
            painter.drawTexture(rect, me.textureFrame, color)
        }

        if !me.movie.numFrames or me.movie.numFrames > 1:
            me.invalidate()
    }
}

impl ImageRect
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null)->*ImageRect
    {
        Debug.assert(parent != null)
        let res = Wnd.create'ImageRect(parent, null, position, id)
        return res
    }

    mtd clear()
    {
        me.getApp().renderer.removeTexture(&me.textureFrame)
    }

    // Set movie as an image list
    mtd setMovie(image: ImageList)
    {
        me.clear()
        me.movie.set(image)
        me.invalidate()
    }

    // Associate image
    #[Swag.Overload]
    mtd setImage(image: ImageList)
    {
        me.clear()
        var tmp = image
        tmp.totalFrames = 1
        tmp.frameSizeX  = tmp.texture.width
        tmp.frameSizeY  = tmp.texture.height
        me.movie.set(tmp)
    }

    // Associate image
    #[Swag.Overload]
    mtd setImage(image: Texture)
    {
        me.clear()
        var tmp: ImageList
        tmp.texture     = image
        tmp.totalFrames = 1
        tmp.frameSizeX  = tmp.texture.width
        tmp.frameSizeY  = tmp.texture.height
        me.movie.set(tmp)
    }

    // Associate image
    #[Swag.Overload]
    mtd setImage(image: Image)
    {
        me.clear()
        var tmp: ImageList
        tmp.texture     = me.getApp().renderer.addImage(image)
        tmp.totalFrames = 1
        tmp.frameSizeX  = tmp.texture.width
        tmp.frameSizeY  = tmp.texture.height
        me.movie.set(tmp)
    }

    // Load and set movie
    mtd setFile(fileName: string) throw
    {
        me.clear()
        try me.movie.set(fileName)
        me.textureFrame = me.getApp().renderer.addImage(me.movie.frame)
        me.invalidate()
    }
}
