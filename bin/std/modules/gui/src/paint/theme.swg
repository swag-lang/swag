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

public struct ThemeImageRect
{
    rect:   Math.Rectangle
    corner: f32
}

public struct ThemeImageRects
{
    wnd_Border:             ThemeImageRect = @{{130,1,27,27}, 3}
    wnd_Caption:            ThemeImageRect = @{{57,1,72,27}, 12}
    wnd_CaptionSmall:       ThemeImageRect = @{{57,1,72,27}, 1}

    dlg_BtnBar:             ThemeImageRect = @{{29,29,27,27}, 1}

    btnPush_Normal:         ThemeImageRect = @{{1,1,27,27}, 9}
    btnPush_NormalBorder:   ThemeImageRect = @{{29,1,27,27}, 9}
    btnPush_Hot:            ThemeImageRect = @{{1,1,27,27}, 9}
    btnOush_HotBorder:      ThemeImageRect = @{{29,1,27,27}, 9}
    btnPush_Pressed:        ThemeImageRect = @{{1,1,27,27}, 9}
    btnPush_PressedBorder:  ThemeImageRect = @{{29,1,27,27}, 9}
    btnPush_Disabled:       ThemeImageRect = @{{1,1,27,27}, 9}
    btnPush_DisabledBorder: ThemeImageRect = @{{29,1,27,27}, 9}

    btnIcon_Normal:         ThemeImageRect = @{{1,29,27,27}, 9}
    btnIcon_Hot:            ThemeImageRect = @{{1,29,27,27}, 9}
    btnIcon_Pressed:        ThemeImageRect = @{{1,29,27,27}, 9}
    btnIcon_Disabled:       ThemeImageRect = @{{1,29,27,27}, 9}
}

public struct ThemeMetrics
{
    wndBorderSize:          f32 = 3
    wndBigCaptionCY:        f32 = 30
    wndSmallCaptionCY:      f32 = 24
    wndCaptionMarginTitle:  f32 = 5
    btnMargins:             f32 = -5
    iconTextMargin:         f32 = 4
}

public struct ThemeColors
{
    wnd_Bk:                         Color = 0xFF424242
    wnd_Caption:                    Color = 0xFF535353
    wnd_CaptionNotActived:          Color = 0xFF535353
    wnd_CaptionText:                Color = 0xFFFFFFFF
    wnd_CaptionNotActivatedText:    Color = 0xFFFFFFFF

    dlg_Bk:                         Color = 0xFF303030
    dlg_BtnBarBk:                   Color = 0xFF353535

    btnPush_DisabledText:           Color = 0x5FFFFFFF
    btnPush_DisabledBorder:         Color = 0x5FFFFFFF
    btnPush_DisabledBk:             Color = 0

    btnPush_Text:                   Color = 0xFFFFFFFF
    btnPush_Border:                 Color = 0xFFFFFFFF
    btnPush_Bk:                     Color = 0
    btnPush_PressedText:            Color = 0xFF424242
    btnPush_PressedBorder:          Color = 0xFFFFFFFF
    btnPush_PressedBk:              Color = 0xFFFFFFFF
    btnPush_HotText:                Color = 0xFF424242
    btnPush_HotBorder:              Color = 0xFFEEEEEE
    btnPush_HotBk:                  Color = 0xFFEEEEEE

    btnPushDefault_Text:            Color = 0xFFFFFFFF
    btnPushDefault_Border:          Color = 0xFF1473e6
    btnPushDefault_Bk:              Color = 0xFF1473e6
    btnPushDefault_PressedText:     Color = 0xFFFFFFFF
    btnPushDefault_PressedBorder:   Color = 0xFF1473e6
    btnPushDefault_PressedBk:       Color = 0xFF1473e6
    btnPushDefault_HotText:         Color = 0xFFFFFFFF
    btnPushDefault_HotBorder:       Color = 0xFF2483f6
    btnPushDefault_HotBk:           Color = 0xFF2483f6

    btnIcon_Icon:                   Color = 0xFFFFFFFF
    btnIcon_Bk:                     Color = 0
    btnIcon_DisabledIcon:           Color = 0xFFFFFFFF
    btnIcon_DisabledBk:             Color = 0
    btnIcon_PressedIcon:            Color = 0xFFFFFFFF
    btnIcon_PressedBk:              Color = 0x3FFFFFFF
    btnIcon_HotIcon:                Color = 0xFFFFFFFF
    btnIcon_HotBk:                  Color = 0x3FEEEEEE

    btnIcon_CloseBk:                Color = 0xFFd61425
    btnIcon_CloseHotBk:             Color = 0xFFe62435
}

struct ThemeResources
{
    imgWidgets:     Texture
    icons24:        ImageList
    typeFaceR:      const *TypeFace
    typeFaceB:      const *TypeFace
    typeFaceI:      const *TypeFace
    typeFaceBI:     const *TypeFace
    fontDefault:    FontFamily
}

struct Theme
{
    res:     ThemeResources
    rects:   ThemeImageRects
    metrics: ThemeMetrics
    colors:  ThemeColors
}

impl Theme
{
    protected mtd setupDefault(renderer: *RenderOgl)
    {
        img := assume Image.decode(DefaultImageName, DefaultImage)
        res.imgWidgets = renderer.addImage(img)

        img = assume Image.decode(DefaultIcons24Name, DefaultIcons24)
        res.icons24.texture = renderer.addImage(img)
        res.icons24.sizeX = 24
        res.icons24.sizeY = 24

        res.typeFaceR  = assume TypeFace.create(DefaultFontRegularName, DefaultFontRegular)
        res.typeFaceB  = assume TypeFace.create(DefaultFontBoldName, DefaultFontBold)
        res.typeFaceI  = assume TypeFace.create(DefaultFontItalicName, DefaultFontItalic)
        res.typeFaceBI = assume TypeFace.create(DefaultFontBoldItalicName, DefaultFontBoldItalic)

        res.fontDefault.regular = Font.create(res.typeFaceR, 13)
        res.fontDefault.bold = Font.create(res.typeFaceB, 13)
        res.fontDefault.italic = Font.create(res.typeFaceI, 13)
        res.fontDefault.boldItalic = Font.create(res.typeFaceBI, 13)
    }

    mtd drawSubRect(painter: *Painter, dstRect: Math.Rectangle, subRect: *ThemeImageRect, color: Color)
    {
        drawSubRect(painter, res.imgWidgets, dstRect, subRect, color)
    }

    func drawSubRect(painter: *Painter, texture: Texture, dstRect: Math.Rectangle, subRect: *ThemeImageRect, color: Color)
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