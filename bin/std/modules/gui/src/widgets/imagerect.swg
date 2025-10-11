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
        if !.movie.isValid() do
            return

        // Update movie, and update resulting texture frame if necessary
        if .movie.update(.getApp().timing.dt)
        {
            if .movie.source != .SpriteSheet
            {
                .getApp().renderer.updateTexture(.textureFrame, .movie.frame.pixels.buffer)
            }
        }

        let painter = evt.bc.painter
        let rect    = .getClientRectPadding()

        let color = .getThemeColors().imageRect_Fg

        #[Swag.Complete]
        switch .movie.source
        {
        case None:
            break
        case SpriteSheet:
            let icon = Icon.from(&.movie.imgList, .movie.frameIndex, rect.width, rect.height)
            icon.paint(painter, rect.x, rect.y, color)
        case Gif:
            painter.drawTexture(rect, .textureFrame, color)
        }

        if !.movie.numFrames or .movie.numFrames > 1 do
            .invalidate()
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
        .getApp().renderer.removeTexture(&.textureFrame)
    }

    // Set movie as an image list
    mtd setMovie(image: ImageList)
    {
        .clear()
        .movie.set(image)
        .invalidate()
    }

    // Associate image
    #[Swag.Overload]
    mtd setImage(image: ImageList)
    {
        .clear()
        var tmp = image
        tmp.totalFrames = 1
        tmp.frameSizeX  = tmp.texture.width
        tmp.frameSizeY  = tmp.texture.height
        .movie.set(tmp)
    }

    // Associate image
    #[Swag.Overload]
    mtd setImage(image: Texture)
    {
        .clear()
        var tmp: ImageList
        tmp.texture     = image
        tmp.totalFrames = 1
        tmp.frameSizeX  = tmp.texture.width
        tmp.frameSizeY  = tmp.texture.height
        .movie.set(tmp)
    }

    // Associate image
    #[Swag.Overload]
    mtd setImage(image: Image)
    {
        .clear()
        var tmp: ImageList
        tmp.texture     = .getApp().renderer.addImage(image)
        tmp.totalFrames = 1
        tmp.frameSizeX  = tmp.texture.width
        tmp.frameSizeY  = tmp.texture.height
        .movie.set(tmp)
    }

    // Load and set movie
    mtd setFile(fileName: string) throw
    {
        .clear()
        try .movie.set(fileName)
        .textureFrame = .getApp().renderer.addImage(.movie.frame)
        .invalidate()
    }
}
