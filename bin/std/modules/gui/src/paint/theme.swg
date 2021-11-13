#global public
using Core, Pixel

protected const DefaultImageName          = "theme/widgets.png"
protected const DefaultFontRegularName    = "theme/segoe_regular.ttf"
protected const DefaultFontBoldName       = "theme/segoe_bold.ttf"
protected const DefaultFontBoldItalicName = "theme/segoe_bold_italic.ttf"
protected const DefaultFontItalicName     = "theme/segoe_italic.ttf"
protected const DefaultIcons24Name        = "theme/icons24.png"
protected const DefaultIcons64Name        = "theme/icons64.png"

protected const DefaultImage          = #load(DefaultImageName)
protected const DefaultFontRegular    = #load(DefaultFontRegularName)
protected const DefaultFontBold       = #load(DefaultFontBoldName)
protected const DefaultFontBoldItalic = #load(DefaultFontBoldItalicName)
protected const DefaultFontItalic     = #load(DefaultFontItalicName)
protected const DefaultIcons24        = #load(DefaultIcons24Name)
protected const DefaultIcons64        = #load(DefaultIcons64Name)

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

    dlg_BtnBar:             ThemeImageRect = @{{29,29,27,27}, 2}

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

    btnCheck_Normal:         ThemeImageRect = @{{57,62,32,32}, 9}
    btnCheck_NormalBorder:   ThemeImageRect = @{{57,29,32,32}, 9}
    btnCheck_Checked:        ThemeImageRect = @{{123,62,32,32}, 9}
    btnCheck_CheckedBorder:  ThemeImageRect = @{{123,29,32,32}, 9}
}

public struct ThemeMetrics
{
    wnd_BorderSize:             f32 = 3
    wnd_BigCaptionCY:           f32 = 30
    wnd_SmallCaptionCY:         f32 = 24
    wnd_CaptionMarginTitle:     f32 = 5

    btn_Margins:                f32 = -5

    iconText_Margin:            f32 = 4

    btnPush_Width:              f32 = 100
    btnPush_Height:             f32 = 30

    btnCheck_Size:              f32 = 26
    btnCheck_Padding:           f32 = 4

    msgDlg_Padding:             f32 = 20
    msgDlg_BtnPadding:          f32 = 5
}

public struct ThemeColors
{
    wnd_Bk:                         Color = 0xFF424242
    wnd_Caption:                    Color = 0xFF535353
    wnd_CaptionNotActived:          Color = 0xFF535353
    wnd_CaptionText:                Color = 0xFFFFFFFF
    wnd_CaptionNotActivatedText:    Color = 0xFFFFFFFF
    wnd_BtnCloseBk:                 Color = 0xFFd61425
    wnd_BtnCloseHotBk:              Color = 0xFFe62435

    dlg_Bk:                         Color = 0xFF303030
    dlg_BtnBarBk:                   Color = 0xFF353535

    btnPush_Text:                   Color = 0xFFFFFFFF
    btnPush_Border:                 Color = 0xFFFFFFFF
    btnPush_Bk:                     Color = 0
    btnPush_PressedText:            Color = 0xFF424242
    btnPush_PressedBorder:          Color = 0xFFFFFFFF
    btnPush_PressedBk:              Color = 0xFFFFFFFF
    btnPush_HotText:                Color = 0xFF424242
    btnPush_HotBorder:              Color = 0xFFEEEEEE
    btnPush_HotBk:                  Color = 0xFFEEEEEE
    btnPush_DisabledText:           Color = 0x5FFFFFFF
    btnPush_DisabledBorder:         Color = 0x5FFFFFFF
    btnPush_DisabledBk:             Color = 0

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

    label_Text:                     Color = 0xFFFFFFFF
    label_Text1:                    Color = 0xFFFF5555
    label_Text2:                    Color = 0xFF55FF55
    label_Text3:                    Color = 0xFFFF5555
    label_DisabledText:             Color = 0x5FFFFFFF
    label_Icon:                     Color = 0xFFFFFFFF
    label_DisabledIcon:             Color = 0xFFFFFFFF

    btnCheck_Text:                  Color = 0xFFFFFFFF
    btnCheck_Border:                Color = 0xFFFFFFFF
    btnCheck_Bk:                    Color = 0
    btnCheck_PressedText:           Color = 0xFFFFFFFF
    btnCheck_PressedBorder:         Color = 0xFFFFFFFF
    btnCheck_PressedBk:             Color = 0x7FFFFFFF
    btnCheck_HotText:               Color = 0xFFFFFFFF
    btnCheck_HotBorder:             Color = 0xFFFFFFFF
    btnCheck_HotBk:                 Color = 0x5FFFFFFF
    btnCheck_DisabledText:          Color = 0x5FFFFFFF
    btnCheck_DisabledBorder:        Color = 0x5FFFFFFF
    btnCheck_DisabledBk:            Color = 0
}

struct ThemeResources
{
    imgWidgets:     Texture
    icons24:        ImageList
    icons64:        ImageList
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

enum ThemeIcons24
{
    WndMinimize
    WndMaximize
    WndClose
}

enum ThemeIcons64
{
    Question
    Warning
    Critical
    Information
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

        img = assume Image.decode(DefaultIcons64Name, DefaultIcons64)
        res.icons64.texture = renderer.addImage(img)
        res.icons64.sizeX = 64
        res.icons64.sizeY = 64

        res.typeFaceR  = assume TypeFace.create(DefaultFontRegularName, DefaultFontRegular)
        res.typeFaceB  = assume TypeFace.create(DefaultFontBoldName, DefaultFontBold)
        res.typeFaceI  = assume TypeFace.create(DefaultFontItalicName, DefaultFontItalic)
        res.typeFaceBI = assume TypeFace.create(DefaultFontBoldItalicName, DefaultFontBoldItalic)

        res.fontDefault.regular = Font.create(res.typeFaceR, 13)
        res.fontDefault.bold = Font.create(res.typeFaceB, 13)
        res.fontDefault.italic = Font.create(res.typeFaceI, 13)
        res.fontDefault.boldItalic = Font.create(res.typeFaceBI, 13)
    }

    mtd getIcon24(idx: ThemeIcons24)->Icon
    {
        var result: retval
        result.imageList = &res.icons24
        result.index = cast(s32) idx
        result.size = 24
        return result
    }

    mtd getIcon64(idx: ThemeIcons64)->Icon
    {
        var result: retval
        result.imageList = &res.icons64
        result.index = cast(s32) idx
        result.size = 64
        return result
    }
}