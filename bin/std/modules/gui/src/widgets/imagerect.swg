#global public
using Core, Pixel

struct ImageRect
{
    using wnd:  Wnd
    imgList:    *ImageList
    freq:       f32 = 0.05
    curFrame:   s32
    curTime:    f32
}

impl IWnd for ImageRect
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        if !imgList
            return

        painter := evt.bc.painter
        theme   := getTheme()

        icon := Icon.from(imgList, curFrame, acast imgList.sizeY)
        icon.paint(painter, 0, 0, getThemeColors().imageRect_Fg)

        curTime += getApp().timing.dt
        if curTime > freq
        {
            numX := imgList.texture.width / imgList.sizeX
            curFrame = (curFrame + 1) % numX
            curTime = 0
        }

        invalidate()
    }
}

impl ImageRect
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle, image: *ImageList, id: WndId = null)->*ImageRect
    {
        Debug.assert(parent != null)
        res := Wnd.create'ImageRect(parent, null, position, id)
        res.imgList = image
        return res
    }
}