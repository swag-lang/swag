#global public
using Core, Pixel

struct ImageRect
{
    using frameWnd: Wnd
    imgList:        ImageList
    freq:           f32 = 0.05
    curFrame:       s32
    curTime:        f32
}

impl IWnd for ImageRect
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        if !imgList.texture.valid
            return

        painter := evt.bc.painter
        theme   := getTheme()

        rect := getClientRectPadding()
        icon := Icon.from(&imgList, curFrame, rect.width, rect.height)
        icon.paint(painter, rect.x, rect.y, getThemeColors().imageRect_Fg)

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
    func create(parent: *Wnd, image: ImageList, position: Math.Rectangle = @{}, id: WndId = null)->*ImageRect
    {
        Debug.assert(parent != null)
        res := Wnd.create'ImageRect(parent, null, position, id)
        res.imgList = image
        return res
    }

    #[Swag.Discardable]
    func create(parent: *Wnd, imagePath: string, position: Math.Rectangle = @{}, id: WndId = null)->*ImageRect
    {
        Debug.assert(parent != null)
        res := Wnd.create'ImageRect(parent, null, position, id)

        img := assume Image.load(imagePath)
        res.imgList.texture = res.getApp().renderer.addImage(img)
        res.imgList.sizeX = img.width
        res.imgList.sizeY = img.height

        return res
    }
}