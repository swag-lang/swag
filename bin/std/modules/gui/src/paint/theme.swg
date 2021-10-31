#global public
using Core, Pixel

protected const DefaultImageName          = "theme/widgets.png"
protected const DefaultFontRegularName    = "theme/segoe_regular.ttf"
protected const DefaultFontBoldName       = "theme/segoe_bold.ttf"
protected const DefaultFontBoldItalicName = "theme/segoe_bold_italic.ttf"
protected const DefaultFontItalicName     = "theme/segoe_italic.ttf"
protected const DefaultIcons24Name        = "theme/icons24.png"

protected const DefaultImage          = #load(DefaultImageName)
protected const DefaultFontRegular    = #load(DefaultFontRegularName)
protected const DefaultFontBold       = #load(DefaultFontBoldName)
protected const DefaultFontBoldItalic = #load(DefaultFontBoldItalicName)
protected const DefaultFontItalic     = #load(DefaultFontItalicName)
protected const DefaultIcons24        = #load(DefaultIcons24Name)

struct ImageSubRect
{
    rect:   Math.Rectangle
    corner: f32
}

struct ImageMetrics
{
    wndBorder:          ImageSubRect = @{{130,1,27,27}, 3}
    wndBigCaption:      ImageSubRect = @{{57,1,72,27}, 1}
    wndSmallCaption:    ImageSubRect = @{{57,1,72,27}, 1}
    wndCaptionMinimize: ImageSubRect = @{{57,29,27,27}, 1}
    wndCaptionMaximize: ImageSubRect = @{{85,29,27,27}, 1}
    wndCaptionClose:    ImageSubRect = @{{113,29,27,27}, 1}

    btnDisabled:        ImageSubRect = @{{1,57,27,27}, 9}
    btnDisabledBorder:  ImageSubRect = @{{29,57,27,27}, 9}
    btnPressed:         ImageSubRect = @{{1,29,27,27}, 9}
    btnPressedBorder:   ImageSubRect = @{{29,29,27,27}, 9}
    btnHot:             ImageSubRect = @{{1,1,27,27}, 9}
    btnHotBorder:       ImageSubRect = @{{29,1,27,27}, 9}
}

struct Theme
{
    imgWidgets:     Texture
    imgMetrics:     ImageMetrics
    icons24:        ImageList
    typeFaceR:      const *TypeFace
    typeFaceB:      const *TypeFace
    typeFaceI:      const *TypeFace
    typeFaceBI:     const *TypeFace
    fontDefault:      FontFamily
    palette:        Palette

    fontDefaultSize:    u32 = 13

    wndBorderSize:          f32 = 3
    wndBigCaptionCY:        f32 = 30
    wndSmallCaptionCY:      f32 = 24
    wndCaptionMarginTitle:  f32 = 5
    btnMargins:             f32 = -5
    iconTextMargin:         f32 = 4
}

impl Theme
{
    protected mtd setupDefault(renderer: *RenderOgl)
    {
        img := assume Image.decode(DefaultImageName, DefaultImage)
        imgWidgets = renderer.addImage(img)

        img = assume Image.decode(DefaultIcons24Name, DefaultIcons24)
        icons24.texture = renderer.addImage(img)
        icons24.sizeX = 24
        icons24.sizeY = 24

        typeFaceR  = assume TypeFace.create(DefaultFontRegularName, DefaultFontRegular)
        typeFaceB  = assume TypeFace.create(DefaultFontBoldName, DefaultFontBold)
        typeFaceI  = assume TypeFace.create(DefaultFontItalicName, DefaultFontItalic)
        typeFaceBI = assume TypeFace.create(DefaultFontBoldItalicName, DefaultFontBoldItalic)

        fontDefault.regular = Font.create(typeFaceR, fontDefaultSize)
        fontDefault.bold = Font.create(typeFaceB, fontDefaultSize)
        fontDefault.italic = Font.create(typeFaceI, fontDefaultSize)
        fontDefault.boldItalic = Font.create(typeFaceBI, fontDefaultSize)
    }

    mtd drawSubRect(painter: *Painter, dstRect: Math.Rectangle, subRect: *ImageSubRect, color: Color)
    {
        drawSubRect(painter, imgWidgets, dstRect, subRect, color)
    }

    func drawSubRect(painter: *Painter, texture: Texture, dstRect: Math.Rectangle, subRect: *ImageSubRect, color: Color)
    {
        if subRect.rect.isEmpty() or dstRect.isEmpty() or color.a == 0
            return

        painter.setInterpolationMode(InterpolationMode.Linear)

        if subRect.corner == 0
        {
            painter.drawTexture(dstRect, subRect.rect, texture, color)
            return
        }

        cx, cy := subRect.corner
        if cx * 2 > dstRect.width
            cx = dstRect.width / 2
        if cy * 2 > dstRect.height
            cy = dstRect.height / 2

        d := dstRect
        s := subRect.rect
        loop i: 9
        {
            switch i
            {
            case 0:
                d.width = cx; d.height = cy
                s.width = cx; s.height = cy
                painter.drawTexture(d, s, texture, color)

            case 1, 7:
                d.x += cx; s.x += cx
                d.width = dstRect.width - (cx * 2)
                s.width = subRect.rect.width - (cx * 2)
                painter.drawTexture(d, s, texture, color)

            case 2, 8:
                d.x = dstRect.x + dstRect.width - cx
                s.x = subRect.rect.x + subRect.rect.width - cx
                d.width = cx; s.width = cx
                painter.drawTexture(d, s, texture, color)

            case 3:
                d.x = dstRect.x; d.y = dstRect.y + cx
                s.x = subRect.rect.x; s.y = subRect.rect.y + cy
                d.width = cx; s.width = cx;
                d.height = dstRect.height - (cy * 2)
                s.height = subRect.rect.height - (cy * 2)
                painter.drawTexture(d, s, texture, color)

            case 4:
                d.x += cx; s.x += cx
                d.width = dstRect.width - (cx * 2)
                d.height = dstRect.height - (cy * 2)
                s.width = subRect.rect.width - (cx * 2)
                s.height = subRect.rect.height - (cy * 2)
                painter.drawTexture(d, s, texture, color)

            case 5:
                d.x += d.width; s.x += s.width
                d.width = cx; s.width = cy
                painter.drawTexture(d, s, texture, color)

            case 6:
                d.x = dstRect.x; d.y = dstRect.y + dstRect.height - cy
                s.x = subRect.rect.x; s.y = subRect.rect.y + subRect.rect.height - cy
                d.width = cx; d.height = cy
                s.width = cx; s.height = cy
                painter.drawTexture(d, s, texture, color)
            }
        }
    }
}