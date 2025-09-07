#global public
using Core, Pixel

internal const DefaultImageName = "theme/widgets.png"

internal const DefaultFontRegularName = "theme/segoeui.ttf"
internal const DefaultFontBoldName = "theme/segoeuib.ttf"
internal const DefaultFontBoldItalicName = "theme/segoeuiz.ttf"
internal const DefaultFontItalicName = "theme/segoeuii.ttf"

internal const DefaultFontFsRegularName = "theme/consola.ttf"
internal const DefaultFontFsBoldName = "theme/consolab.ttf"
internal const DefaultFontFsBoldItalicName = "theme/consolaz.ttf"
internal const DefaultFontFsItalicName = "theme/consolai.ttf"

internal const DefaultIcons24Name = "theme/icons24.png"
internal const DefaultIcons64Name = "theme/icons64.png"
internal const DefaultSpinName = "theme/spin.png"

internal const DefaultImage = #include(DefaultImageName)
internal const DefaultFontRegular = #include(DefaultFontRegularName)
internal const DefaultFontBold = #include(DefaultFontBoldName)
internal const DefaultFontBoldItalic = #include(DefaultFontBoldItalicName)
internal const DefaultFontItalic = #include(DefaultFontItalicName)
internal const DefaultFontFsRegular = #include(DefaultFontFsRegularName)
internal const DefaultFontFsBold = #include(DefaultFontFsBoldName)
internal const DefaultFontFsBoldItalic = #include(DefaultFontFsBoldItalicName)
internal const DefaultFontFsItalic = #include(DefaultFontFsItalicName)
internal const DefaultIcons24 = #include(DefaultIcons24Name)
internal const DefaultIcons64 = #include(DefaultIcons64Name)
internal const DefaultSpin = #include(DefaultSpinName)

struct ThemeResources
{
    imgWidgets:              Texture
    icons24:                 *ImageList
    icons64:                 *ImageList
    spin:                    *ImageList
    defaultTypeFaceR:        *TypeFace
    defaultTypeFaceB:        *TypeFace
    defaultTypeFaceI:        *TypeFace
    defaultTypeFaceBI:       *TypeFace
    defaultFsTypeFaceR:      *TypeFace
    defaultFsTypeFaceB:      *TypeFace
    defaultFsTypeFaceI:      *TypeFace
    defaultFsTypeFaceBI:     *TypeFace
    fontDefault:             FontFamily
    fontDefaultFs:           FontFamily
}

struct Theme
{
    intialized:     bool
    res:            ThemeResources
    rects:          ThemeImageRects
    metrics:        ThemeMetrics
    colors:         ThemeColors
}

impl Theme
{
    internal mtd freeDefault()
    {
        Memory.delete(me.res.icons24)
        Memory.delete(me.res.icons64)
        Memory.delete(me.res.spin)
    }

    internal mtd setupDefault(renderer: *RenderOgl)
    {
        me.intialized = true
        me.setLight()

        with me.res
        {
            var img = assume Image.decode(DefaultImageName, DefaultImage)
            .imgWidgets = renderer.addImage(img)

            img      = assume Image.decode(DefaultIcons24Name, DefaultIcons24)
            .icons24 = Memory.new'ImageList()
            .icons24.set(renderer.addImage(img), 24, 24)

            img      = assume Image.decode(DefaultIcons64Name, DefaultIcons64)
            .icons64 = Memory.new'ImageList()
            .icons64.set(renderer.addImage(img), 64, 64)

            img   = assume Image.decode(DefaultSpinName, DefaultSpin)
            .spin = Memory.new'ImageList()
            .spin.set(renderer.addImage(img), 48, 48)

            .defaultTypeFaceR  = assume TypeFace.create(DefaultFontRegularName, DefaultFontRegular)
            .defaultTypeFaceB  = assume TypeFace.create(DefaultFontBoldName, DefaultFontBold)
            .defaultTypeFaceI  = assume TypeFace.create(DefaultFontItalicName, DefaultFontItalic)
            .defaultTypeFaceBI = assume TypeFace.create(DefaultFontBoldItalicName, DefaultFontBoldItalic)

            .defaultFsTypeFaceR  = assume TypeFace.create(DefaultFontFsRegularName, DefaultFontFsRegular)
            .defaultFsTypeFaceB  = assume TypeFace.create(DefaultFontFsBoldName, DefaultFontFsBold)
            .defaultFsTypeFaceI  = assume TypeFace.create(DefaultFontFsItalicName, DefaultFontFsItalic)
            .defaultFsTypeFaceBI = assume TypeFace.create(DefaultFontFsBoldItalicName, DefaultFontFsBoldItalic)

            .fontDefault.regular    = Font.create(.defaultTypeFaceR, me.metrics.defaultFontSize)
            .fontDefault.bold       = Font.create(.defaultTypeFaceB, me.metrics.defaultFontSize)
            .fontDefault.italic     = Font.create(.defaultTypeFaceI, me.metrics.defaultFontSize)
            .fontDefault.boldItalic = Font.create(.defaultTypeFaceBI, me.metrics.defaultFontSize)

            .fontDefaultFs.regular    = Font.create(.defaultFsTypeFaceR, me.metrics.defaultFixedFontSize)
            .fontDefaultFs.bold       = Font.create(.defaultFsTypeFaceB, me.metrics.defaultFixedFontSize)
            .fontDefaultFs.italic     = Font.create(.defaultFsTypeFaceI, me.metrics.defaultFixedFontSize)
            .fontDefaultFs.boldItalic = Font.create(.defaultFsTypeFaceBI, me.metrics.defaultFixedFontSize)
        }
    }

    mtd setDark()
    {
        const Transparent    = 0x00000000
        const Blue           = 0xFF1473E6
        const LightBlue      = 0xFF2483F6
        const Disabled       = 0x5FFFFFFF
        const LighterSmall   = 0x0FFFFFFF
        const LighterMedium  = 0x1FFFFFFF
        const LighterBig     = 0x2FFFFFFF
        const LighterVeryBig = 0x3FFFFFFF
        const White          = 0xFFF0F0F0
        const Gray1          = 0xFFE0E0E0
        const Gray2          = 0xFF808080
        const Gray3          = 0xFF606060
        const Gray4          = 0xFF505050
        const Gray5          = 0xFF404040
        const Gray6          = 0xFF353535
        const Gray7          = 0xFF303030
        const Gray8          = 0xFF202020

        with me.colors
        {
            .richColor1 = Argb.Red
            .richColor2 = Argb.Green
            .richColor3 = Argb.Blue

            .surfaceDisabled = Color.fromArgb(0xFFBBBBBB)

            .hilight        = Blue
            .hilightLight   = Color.getLighter(Color.fromArgb(Blue), 0.2)
            .hilightDark    = Color.getDarker(Color.fromArgb(Blue), 0.2)
            .disabled       = Disabled
            .transparent    = Transparent
            .lighterSmall   = LighterSmall
            .lighterMedium  = LighterMedium
            .lighterBig     = LighterBig
            .lighterVeryBig = LighterVeryBig

            .wnd_Bk                      = Gray5
            .wnd_Caption                 = Gray4
            .wnd_CaptionNotActived       = Gray4
            .wnd_CaptionText             = White
            .wnd_CaptionNotActivatedText = White
            .wnd_BtnCloseBk              = 0xFFD61425
            .wnd_BtnCloseHotBk           = 0xFFE62435
            .wnd_Shadow                  = 0x8C000000

            .view_Bk = 0xFF282828

            .dlg_Bk       = Gray6
            .dlg_BtnBarBk = Gray7

            .btnPush_StrongText   = White
            .btnPush_StrongBorder = White
            .btnPush_StrongBk     = Transparent

            .btnPush_Text           = White
            .btnPush_Border         = Gray2
            .btnPush_Bk             = Transparent
            .btnPush_PressedText    = Gray5
            .btnPush_PressedBorder  = White
            .btnPush_PressedBk      = White
            .btnPush_HotText        = Gray5
            .btnPush_HotBorder      = Gray1
            .btnPush_HotBk          = Gray1
            .btnPush_DisabledText   = Disabled
            .btnPush_DisabledBorder = Disabled
            .btnPush_DisabledBk     = Transparent

            .btnPushFlat_DisabledText   = Disabled
            .btnPushFlat_DisabledBorder = Disabled
            .btnPushFlat_DisabledBk     = Transparent
            .btnPushFlat_Text           = White
            .btnPushFlat_Border         = Transparent
            .btnPushFlat_Bk             = Transparent
            .btnPushFlat_PressedText    = White
            .btnPushFlat_PressedBorder  = Transparent
            .btnPushFlat_PressedBk      = Gray3
            .btnPushFlat_HotText        = White
            .btnPushFlat_HotBorder      = Transparent
            .btnPushFlat_HotBk          = Gray4

            .btnPushDefault_DisabledText   = Disabled
            .btnPushDefault_DisabledBorder = Disabled
            .btnPushDefault_DisabledBk     = Transparent
            .btnPushDefault_Text           = White
            .btnPushDefault_Border         = Blue
            .btnPushDefault_Bk             = Blue
            .btnPushDefault_PressedText    = White
            .btnPushDefault_PressedBorder  = Blue
            .btnPushDefault_PressedBk      = Blue
            .btnPushDefault_HotText        = White
            .btnPushDefault_HotBorder      = LightBlue
            .btnPushDefault_HotBk          = LightBlue

            .btnIcon_Icon         = White
            .btnIcon_Text         = White
            .btnIcon_Bk           = Transparent
            .btnIcon_DisabledIcon = Disabled
            .btnIcon_DisabledText = Disabled
            .btnIcon_DisabledBk   = Transparent
            .btnIcon_PressedIcon  = White
            .btnIcon_PressedText  = White
            .btnIcon_PressedBk    = LighterBig
            .btnIcon_HotIcon      = White
            .btnIcon_HotText      = White
            .btnIcon_HotBk        = LighterMedium

            .btnIcon_CheckedDisabledIcon = Disabled
            .btnIcon_CheckedDisabledText = Disabled
            .btnIcon_CheckedPressedIcon  = White
            .btnIcon_CheckedPressedText  = White
            .btnIcon_CheckedHotIcon      = White
            .btnIcon_CheckedHotText      = White
            .btnIcon_CheckedIcon         = White
            .btnIcon_CheckedText         = White
            .btnIcon_CheckedDisabledBk   = Transparent
            .btnIcon_CheckedPressedBk    = LighterBig
            .btnIcon_CheckedHotBk        = Transparent
            .btnIcon_CheckedBk           = Transparent
            .btnIcon_CheckedMark         = Color.getLighter(Color.fromArgb(Blue), 0.2)
            .btnIcon_CheckedPressedMark  = Blue
            .btnIcon_CheckedHotMark      = Color.getLighter(Color.fromArgb(Blue), 0.1)
            .btnIcon_CheckedDisabledMark = Disabled

            .label_Text         = Gray1
            .label_TextLight    = Gray2
            .label_Text1        = 0xFFFF5555
            .label_Text2        = 0xFF55FF55
            .label_Text3        = 0xFFFF5555
            .label_DisabledText = Disabled
            .label_Icon         = White
            .label_DisabledIcon = White

            .btnCheck_Text           = White
            .btnCheck_Border         = White
            .btnCheck_Mark           = White
            .btnCheck_Bk             = Transparent
            .btnCheck_PressedText    = White
            .btnCheck_PressedBorder  = White
            .btnCheck_PressedMark    = White
            .btnCheck_PressedBk      = LighterBig
            .btnCheck_HotText        = White
            .btnCheck_HotBorder      = White
            .btnCheck_HotMark        = White
            .btnCheck_HotBk          = LighterSmall
            .btnCheck_DisabledText   = Disabled
            .btnCheck_DisabledBorder = Disabled
            .btnCheck_DisabledMark   = Disabled
            .btnCheck_DisabledBk     = Transparent
            .btnCheck_CheckedText    = White
            .btnCheck_CheckedBk      = Transparent
            .btnCheck_CheckedBorder  = White
            .btnCheck_CheckedMark    = White

            .toolTip_Bk     = 0xFFCCCC88
            .toolTip_Border = 0xFF000000
            .toolTip_Text   = 0xFF222222

            .url_Text    = Blue
            .url_HotText = Color.getLighter(Color.fromArgb(Blue), 0.2)

            .edit_Border         = Gray3
            .edit_FocusBorder    = Blue
            .edit_HotBorder      = Gray3
            .edit_DisabledBorder = Gray3
            .edit_Bk             = Gray6
            .edit_DisabledBk     = Gray5
            .edit_ErrorBk        = Gray6
            .edit_HotBk          = Gray6
            .edit_FocusBk        = Gray6
            .edit_Note           = Disabled
            .edit_Text           = White
            .edit_TextUnit       = Disabled
            .edit_DisabledText   = Disabled
            .edit_SelectedText   = White
            .edit_SelectedBk     = Blue
            .edit_ErrorBorder    = 0xFFFF0000

            .richEdit_Bk             = Gray8
            .richEdit_TextSpecial    = Gray7
            .richEdit_Text           = White
            .richEdit_SelText        = Transparent
            .richEdit_SelTextNoFocus = Transparent
            .richEdit_SelBk          = Color.getDarker(Color.fromArgb(Blue), 0.5)
            .richEdit_SelBkNoFocus   = Gray6
            .richEdit_Caret          = White
            .richEdit_CurLineBorder  = Gray4

            .btnToggle_DisabledBorder   = Disabled
            .btnToggle_DisabledMark     = Disabled
            .btnToggle_DisabledBk       = Transparent
            .btnToggle_OffBorder        = White
            .btnToggle_OffMark          = White
            .btnToggle_OffBk            = Transparent
            .btnToggle_OffHotBorder     = White
            .btnToggle_OffHotMark       = White
            .btnToggle_OffHotBk         = LighterSmall
            .btnToggle_OffPressedBorder = White
            .btnToggle_OffPressedMark   = White
            .btnToggle_OffPressedBk     = LighterBig
            .btnToggle_OnBorder         = White
            .btnToggle_OnMark           = White
            .btnToggle_OnBk             = Blue
            .btnToggle_OnHotBorder      = White
            .btnToggle_OnHotMark        = White
            .btnToggle_OnHotBk          = LightBlue
            .btnToggle_OnPressedBorder  = White
            .btnToggle_OnPressedMark    = White
            .btnToggle_OnPressedBk      = Blue

            .slider_BkLeft        = Gray6
            .slider_BkMiddle      = Gray6
            .slider_BkRight       = Gray6
            .slider_HilightBk     = Blue
            .slider_Mark          = Blue
            .slider_HotMark       = White
            .slider_PressedMark   = White
            .slider_DisabledMark  = Gray4
            .slider_DisabledBk    = Gray6
            .slider_StepBk        = Color.getLighter(Color.fromArgb(Gray6), 0.1)
            .slider_HilightStepBk = Color.getDarker(Color.fromArgb(Blue), 0.1)
            .slider_HotStepBk     = Color.getDarker(Color.fromArgb(White), 0.3)

            .circular_Bk             = Transparent
            .circular_DisabledBk     = Transparent
            .circular_Circle         = Gray1
            .circular_DisabledCircle = Gray1
            .circular_Mark           = Blue
            .circular_HotMark        = White
            .circular_PressedMark    = White
            .circular_DisabledMark   = Gray4
            .circular_Step           = Gray2
            .circular_DisabledStep   = Gray2
            .circular_RangeArc       = Blue

            .tabItem_DisabledText             = Gray4
            .tabItem_SelectedText             = White
            .tabItem_UnSelectedText           = Color.getDarker(Color.fromArgb(White), 0.3)
            .tabItem_FlatSelected             = Blue
            .tabItem_FlatHot                  = White
            .tabItem_RoundSelected            = Gray6
            .tabItem_RoundHot                 = Gray4
            .tabItem_ButtonSelected           = Gray4
            .tabItem_ButtonHot                = Gray5
            .tabItem_PushButtonSelected       = Blue
            .tabItem_PushButtonSelectedBorder = White
            .tabItem_PushButtonHot            = Gray5
            .tabItem_PushButtonHotBorder      = Gray2
            .tabItem_PushButtonBk             = Gray6
            .tabItem_PushButtonBkBorder       = Gray3
            .tabBar_FlatBk                    = Gray4
            .tabBar_RoundBk                   = Gray6
            .tabView_FlatBk                   = Gray6
            .tabView_RoundBk                  = Gray6

            .menuPopup_Bk        = Gray6
            .menuPopup_Border    = Color.getLighter(Color.fromArgb(Gray6), 0.2)
            .menuPopup_Shadow    = 0xFF000000
            .menuPopup_Separator = Color.getLighter(Color.fromArgb(Gray6), 0.2)
            .menuBar_Bk          = Gray4
            .menuBar_Border      = 0
            .menuBar_SelectedBk  = 0x1F000000

            .popupItem_Text         = Gray1
            .popupItem_RightText    = Gray2
            .popupItem_DisabledText = Gray2
            .popupItem_HotText      = White
            .popupItem_HotRightText = White
            .popupItem_HotBk        = Blue

            .scrollBar_Bk         = Gray6
            .scrollBar_HotBk      = 0x10888888
            .scrollBar_DisabledBk = 0
            .scrollBar_Box        = Gray3
            .scrollBar_HotBox     = Gray2
            .scrollBar_PressedBox = Gray1

            .comboBox_Border           = Gray3
            .comboBox_HotBorder        = Gray2
            .comboBox_DisabledBorder   = Gray3
            .comboBox_SelectedBorder   = Gray2
            .comboBox_Bk               = Gray6
            .comboBox_HotBk            = Color.getDarker(Color.fromArgb(Gray5), 0.05)
            .comboBox_DisabledBk       = Gray5
            .comboBox_SelectedBk       = Gray6
            .comboBox_Text             = Gray1
            .comboBox_HotText          = White
            .comboBox_DisabledText     = Gray3
            .comboBox_SelectedText     = White
            .comboBoxItem_Text         = Gray1
            .comboBoxItem_HotBk        = Blue
            .comboBoxItem_HotText      = White
            .comboBoxItem_CheckMark    = Blue
            .comboBoxItem_HotCheckMark = White

            .popupList_Separator    = Gray4
            .popupList_HotBk        = Blue
            .popupList_Text         = Gray1
            .popupList_HotText      = White
            .popupList_CheckMark    = Blue
            .popupList_HotCheckMark = White

            .frameWnd_SmallShadow    = 0x50000000
            .frameWnd_Border         = Gray3
            .frameWnd_PopupBorder    = Gray2
            .frameWnd_FocusBorder    = Blue
            .frameWnd_DisabledBorder = Gray3
            .frameWnd_Bk             = Gray6
            .frameWnd_DisabledBk     = Gray5
            .frameWnd_FocusBk        = Gray6

            .listItem_FocusSelectedBk = Blue
            .listItem_SelectedBk      = Gray5
            .listItem_HotBk           = Gray5
            .listItem_DisabledText    = Gray3
            .listItem_SelectedText    = White
            .listItem_Text            = White
            .listItem_Separator       = Color.getDarker(Color.fromArgb(White), 0.3)
            .listItem_AlternateBk     = 0x08FFFFFF
            .listItem_Line            = Gray4

            .headerItem_DisabledText = Gray4
            .headerItem_HotText      = White
            .headerItem_HotBk        = 0x10FFFFFF
            .headerItem_ClickedBk    = 0x20FFFFFF
            .headerItem_Text         = White
            .headerItem_Separator    = Color.getLighter(Color.fromArgb(Gray4), 0.2)
            .header_Bk               = Gray4

            .splitView_TitleBk = Gray6
            .splitView_Bar     = Gray4
            .splitView_BarHot  = Blue

            .progressBar_DisabledBorder = Gray3
            .progressBar_DisabledBk     = Transparent
            .progressBar_DisabledMark   = Gray3
            .progressBar_Border         = Gray2
            .progressBar_Bk             = Gray6
            .progressBar_Mark           = Blue

            .palPicker_Border    = Gray7
            .palPicker_HotBorder = Blue

            .colorPicker_Border    = 0x3F000000
            .colorPicker_BackAlpha = Gray4
            .colorPicker_Mark      = Gray1

            .imageRect_Fg = White

            .embInfo_CriticalBk      = 0xFF8F0000
            .embInfo_CriticalText    = White
            .embInfo_InformationBk   = 0xFF303050
            .embInfo_InformationText = White
            .embInfo_MessageBk       = 0xFFAD8F1B
            .embInfo_MessageText     = Gray5

            .prop_FocusBk     = Color.getLighter(Color.fromArgb(Gray5), 0.05)
            .prop_FocusBorder = Gray3
            .prop_HotBk       = Color.getDarker(Color.fromArgb(Gray5), 0.03)
            .prop_HotBorder   = Transparent
            .prop_GridBorder  = Gray7
            .prop_GridColumn  = Gray7
            .prop_SectionBk   = Gray6
            .prop_CategBk     = Transparent
            .prop_Bk1         = Color.getDarker(Color.fromArgb(Gray5), 0.06)
        }
    }

    mtd setLight()
    {
        const Transparent    = 0x00000000
        const Blue           = 0xFF1473E6
        const LightBlue      = 0xFF2483F6
        const Disabled       = 0x5F000000
        const LighterSmall   = 0x10000000
        const LighterMedium  = 0x20000000
        const LighterBig     = 0x30000000
        const LighterVeryBig = 0x40000000
        const White          = 0xFFFFFFFF
        const Gray1          = 0xFF303030     // primary text
        const Gray2          = 0xFF404040     // secondary text
        const Gray3          = 0xFF606060     // muted text / strong borders
        const Gray4          = 0xFF808080     // subtle icons / secondary borders
        const Gray5          = 0xFFA0A0A0     // disabled-ish strokes
        const Gray6          = 0xFFCCCCCC     // control borders
        const Gray7          = 0xFFE6E6E6     // panel fills
        const Gray8          = 0xFFF6F6F6     // app background / views

        with me.colors
        {
            .richColor1 = Argb.Red
            .richColor2 = Argb.Green
            .richColor3 = Argb.Blue

            .surfaceDisabled = Color.fromArgb(0xFFEEEEEE)

            .hilight        = Blue
            .hilightLight   = Color.getLighter(Color.fromArgb(Blue), 0.2)
            .hilightDark    = Color.getDarker(Color.fromArgb(Blue), 0.2)
            .disabled       = Disabled
            .transparent    = Transparent
            .lighterSmall   = LighterSmall
            .lighterMedium  = LighterMedium
            .lighterBig     = LighterBig
            .lighterVeryBig = LighterVeryBig

            .wnd_Bk                      = Gray8
            .wnd_Caption                 = Gray7
            .wnd_CaptionNotActived       = Gray7
            .wnd_CaptionText             = Gray1
            .wnd_CaptionNotActivatedText = Gray2
            .wnd_BtnCloseBk              = 0xFFD61425
            .wnd_BtnCloseHotBk           = 0xFFE62435
            .wnd_Shadow                  = 0x33000000

            .view_Bk = White

            .dlg_Bk       = Gray8
            .dlg_BtnBarBk = Gray7

            .btnPush_StrongText   = Gray1
            .btnPush_StrongBorder = Gray1
            .btnPush_StrongBk     = Transparent

            .btnPush_Text           = Gray1
            .btnPush_Border         = Gray6
            .btnPush_Bk             = Transparent
            .btnPush_PressedText    = Gray1
            .btnPush_PressedBorder  = Gray3
            .btnPush_PressedBk      = Gray7
            .btnPush_HotText        = Gray1
            .btnPush_HotBorder      = Gray6
            .btnPush_HotBk          = Gray8
            .btnPush_DisabledText   = Disabled
            .btnPush_DisabledBorder = Disabled
            .btnPush_DisabledBk     = Transparent

            .btnPushFlat_DisabledText   = Disabled
            .btnPushFlat_DisabledBorder = Disabled
            .btnPushFlat_DisabledBk     = Transparent
            .btnPushFlat_Text           = Gray1
            .btnPushFlat_Border         = Transparent
            .btnPushFlat_Bk             = Transparent
            .btnPushFlat_PressedText    = Gray1
            .btnPushFlat_PressedBorder  = Transparent
            .btnPushFlat_PressedBk      = Gray7
            .btnPushFlat_HotText        = Gray1
            .btnPushFlat_HotBorder      = Transparent
            .btnPushFlat_HotBk          = Gray8

            .btnPushDefault_DisabledText   = Disabled
            .btnPushDefault_DisabledBorder = Disabled
            .btnPushDefault_DisabledBk     = Transparent
            .btnPushDefault_Text           = White
            .btnPushDefault_Border         = Blue
            .btnPushDefault_Bk             = Blue
            .btnPushDefault_PressedText    = White
            .btnPushDefault_PressedBorder  = Blue
            .btnPushDefault_PressedBk      = Blue
            .btnPushDefault_HotText        = White
            .btnPushDefault_HotBorder      = LightBlue
            .btnPushDefault_HotBk          = LightBlue

            .btnIcon_Icon         = Gray1
            .btnIcon_Text         = Gray1
            .btnIcon_Bk           = Transparent
            .btnIcon_DisabledIcon = Disabled
            .btnIcon_DisabledText = Disabled
            .btnIcon_DisabledBk   = Transparent
            .btnIcon_PressedIcon  = Gray1
            .btnIcon_PressedText  = Gray1
            .btnIcon_PressedBk    = LighterBig
            .btnIcon_HotIcon      = Gray1
            .btnIcon_HotText      = Gray1
            .btnIcon_HotBk        = LighterMedium

            .btnIcon_CheckedDisabledIcon = Disabled
            .btnIcon_CheckedDisabledText = Disabled
            .btnIcon_CheckedPressedIcon  = Gray1
            .btnIcon_CheckedPressedText  = Gray1
            .btnIcon_CheckedHotIcon      = Gray1
            .btnIcon_CheckedHotText      = Gray1
            .btnIcon_CheckedIcon         = Gray1
            .btnIcon_CheckedText         = Gray1
            .btnIcon_CheckedDisabledBk   = Transparent
            .btnIcon_CheckedPressedBk    = LighterBig
            .btnIcon_CheckedHotBk        = Transparent
            .btnIcon_CheckedBk           = Transparent
            .btnIcon_CheckedMark         = Color.getLighter(Color.fromArgb(Blue), 0.2)
            .btnIcon_CheckedPressedMark  = Blue
            .btnIcon_CheckedHotMark      = Color.getLighter(Color.fromArgb(Blue), 0.1)
            .btnIcon_CheckedDisabledMark = Disabled

            .label_Text         = Gray1
            .label_TextLight    = Gray2
            .label_Text1        = 0xFFCC0000
            .label_Text2        = 0xFF008A00
            .label_Text3        = 0xFFCC0000
            .label_DisabledText = Disabled
            .label_Icon         = Gray2
            .label_DisabledIcon = Gray5

            .btnCheck_Text           = Gray1
            .btnCheck_Border         = Gray3
            .btnCheck_Mark           = Gray1
            .btnCheck_Bk             = Transparent
            .btnCheck_PressedText    = Gray1
            .btnCheck_PressedBorder  = Gray3
            .btnCheck_PressedMark    = Gray1
            .btnCheck_PressedBk      = LighterBig
            .btnCheck_HotText        = Gray1
            .btnCheck_HotBorder      = Gray3
            .btnCheck_HotMark        = Gray1
            .btnCheck_HotBk          = LighterSmall
            .btnCheck_DisabledText   = Disabled
            .btnCheck_DisabledBorder = Disabled
            .btnCheck_DisabledMark   = Disabled
            .btnCheck_DisabledBk     = Transparent
            .btnCheck_CheckedText    = Gray1
            .btnCheck_CheckedBk      = Transparent
            .btnCheck_CheckedBorder  = Gray3
            .btnCheck_CheckedMark    = Gray1

            .toolTip_Bk     = 0xFFFFFFE0
            .toolTip_Border = 0xFFAAAAAA
            .toolTip_Text   = Gray1

            .url_Text    = Blue
            .url_HotText = Color.getLighter(Color.fromArgb(Blue), 0.2)

            .edit_Border         = Gray6
            .edit_FocusBorder    = Blue
            .edit_HotBorder      = Gray6
            .edit_DisabledBorder = Gray7
            .edit_Bk             = White
            .edit_DisabledBk     = Gray8
            .edit_ErrorBk        = White
            .edit_HotBk          = White
            .edit_FocusBk        = White
            .edit_Note           = Gray5
            .edit_Text           = Gray1
            .edit_TextUnit       = Gray4
            .edit_DisabledText   = Gray5
            .edit_SelectedText   = White
            .edit_SelectedBk     = Blue
            .edit_ErrorBorder    = 0xFFFF0000

            .richEdit_Bk             = Gray8
            .richEdit_TextSpecial    = Gray3
            .richEdit_Text           = Gray1
            .richEdit_SelText        = White
            .richEdit_SelTextNoFocus = Gray1
            .richEdit_SelBk          = Color.getDarker(Color.fromArgb(Blue), 0.2)
            .richEdit_SelBkNoFocus   = Gray7
            .richEdit_Caret          = Gray1
            .richEdit_CurLineBorder  = Gray7

            .btnToggle_DisabledBorder   = Disabled
            .btnToggle_DisabledMark     = Disabled
            .btnToggle_DisabledBk       = Transparent
            .btnToggle_OffBorder        = Gray3
            .btnToggle_OffMark          = Gray3
            .btnToggle_OffBk            = Transparent
            .btnToggle_OffHotBorder     = Gray3
            .btnToggle_OffHotMark       = Gray3
            .btnToggle_OffHotBk         = LighterSmall
            .btnToggle_OffPressedBorder = Gray3
            .btnToggle_OffPressedMark   = Gray3
            .btnToggle_OffPressedBk     = LighterBig
            .btnToggle_OnBorder         = Blue
            .btnToggle_OnMark           = White
            .btnToggle_OnBk             = Blue
            .btnToggle_OnHotBorder      = LightBlue
            .btnToggle_OnHotMark        = White
            .btnToggle_OnHotBk          = LightBlue
            .btnToggle_OnPressedBorder  = Blue
            .btnToggle_OnPressedMark    = White
            .btnToggle_OnPressedBk      = Blue

            .slider_BkLeft        = Gray7
            .slider_BkMiddle      = Gray7
            .slider_BkRight       = Gray7
            .slider_HilightBk     = Blue
            .slider_Mark          = Blue
            .slider_HotMark       = White
            .slider_PressedMark   = White
            .slider_DisabledMark  = Gray5
            .slider_DisabledBk    = Gray8
            .slider_StepBk        = Color.getDarker(Color.fromArgb(Gray7), 0.08)
            .slider_HilightStepBk = Color.getDarker(Color.fromArgb(Blue), 0.1)
            .slider_HotStepBk     = Color.getDarker(Color.fromArgb(Gray7), 0.2)

            .circular_Bk             = Transparent
            .circular_DisabledBk     = Transparent
            .circular_Circle         = Gray6
            .circular_DisabledCircle = Gray7
            .circular_Mark           = Blue
            .circular_HotMark        = White
            .circular_PressedMark    = White
            .circular_DisabledMark   = Gray5
            .circular_Step           = Gray5
            .circular_DisabledStep   = Gray6
            .circular_RangeArc       = Blue

            .tabItem_DisabledText             = Gray5
            .tabItem_SelectedText             = Gray1
            .tabItem_UnSelectedText           = Color.getDarker(Color.fromArgb(Gray1), 0.3)
            .tabItem_FlatSelected             = Blue
            .tabItem_FlatHot                  = Gray1
            .tabItem_RoundSelected            = Gray8
            .tabItem_RoundHot                 = Gray7
            .tabItem_ButtonSelected           = Gray8
            .tabItem_ButtonHot                = Gray7
            .tabItem_PushButtonSelected       = Blue
            .tabItem_PushButtonSelectedBorder = White
            .tabItem_PushButtonHot            = Gray7
            .tabItem_PushButtonHotBorder      = Gray6
            .tabItem_PushButtonBk             = Gray8
            .tabItem_PushButtonBkBorder       = Gray7
            .tabBar_FlatBk                    = Gray7
            .tabBar_RoundBk                   = Gray8
            .tabView_FlatBk                   = Gray8
            .tabView_RoundBk                  = Gray8

            .menuPopup_Bk        = White
            .menuPopup_Border    = Gray7
            .menuPopup_Shadow    = 0x33000000
            .menuPopup_Separator = Gray7
            .menuBar_Bk          = Gray7
            .menuBar_Border      = 0
            .menuBar_SelectedBk  = 0x10000000

            .popupItem_Text         = Gray1
            .popupItem_RightText    = Gray3
            .popupItem_DisabledText = Gray5
            .popupItem_HotText      = White
            .popupItem_HotRightText = White
            .popupItem_HotBk        = Blue

            .scrollBar_Bk         = Gray8
            .scrollBar_HotBk      = 0x10000000
            .scrollBar_DisabledBk = 0
            .scrollBar_Box        = Gray6
            .scrollBar_HotBox     = Gray5
            .scrollBar_PressedBox = Gray4

            .comboBox_Border           = Gray6
            .comboBox_HotBorder        = Gray4
            .comboBox_DisabledBorder   = Gray7
            .comboBox_SelectedBorder   = Gray4
            .comboBox_Bk               = White
            .comboBox_HotBk            = Color.getDarker(Color.fromArgb(White), 0.03)
            .comboBox_DisabledBk       = Gray8
            .comboBox_SelectedBk       = White
            .comboBox_Text             = Gray1
            .comboBox_HotText          = Gray1
            .comboBox_DisabledText     = Gray5
            .comboBox_SelectedText     = Gray1
            .comboBoxItem_Text         = Gray1
            .comboBoxItem_HotBk        = Blue
            .comboBoxItem_HotText      = White
            .comboBoxItem_CheckMark    = Blue
            .comboBoxItem_HotCheckMark = White

            .popupList_Separator    = Gray7
            .popupList_HotBk        = Blue
            .popupList_Text         = Gray1
            .popupList_HotText      = White
            .popupList_CheckMark    = Blue
            .popupList_HotCheckMark = White

            .frameWnd_SmallShadow    = 0x25000000
            .frameWnd_Border         = Gray6
            .frameWnd_PopupBorder    = Gray5
            .frameWnd_FocusBorder    = Blue
            .frameWnd_DisabledBorder = Gray7
            .frameWnd_Bk             = Gray8
            .frameWnd_DisabledBk     = Gray8
            .frameWnd_FocusBk        = Gray8

            .listItem_FocusSelectedBk = Blue
            .listItem_SelectedBk      = Color.getDarker(Color.fromArgb(Gray8), 0.06)
            .listItem_HotBk           = Color.getDarker(Color.fromArgb(Gray8), 0.04)
            .listItem_DisabledText    = Gray5
            .listItem_SelectedText    = Gray1
            .listItem_Text            = Gray1
            .listItem_Separator       = Color.getDarker(Color.fromArgb(Gray8), 0.12)
            .listItem_AlternateBk     = 0x08000000
            .listItem_Line            = Gray7

            .headerItem_DisabledText = Gray5
            .headerItem_HotText      = Gray1
            .headerItem_HotBk        = 0x10000000
            .headerItem_ClickedBk    = 0x20000000
            .headerItem_Text         = Gray1
            .headerItem_Separator    = Color.getLighter(Color.fromArgb(Gray7), 0.2)
            .header_Bk               = Gray7

            .splitView_TitleBk = Gray8
            .splitView_Bar     = Gray7
            .splitView_BarHot  = Blue

            .progressBar_DisabledBorder = Gray7
            .progressBar_DisabledBk     = Transparent
            .progressBar_DisabledMark   = Gray6
            .progressBar_Border         = Gray6
            .progressBar_Bk             = Gray8
            .progressBar_Mark           = Blue

            .palPicker_Border    = Gray6
            .palPicker_HotBorder = Blue

            .colorPicker_Border    = 0x1A000000
            .colorPicker_BackAlpha = Gray7
            .colorPicker_Mark      = Gray3

            .imageRect_Fg = Gray1

            .embInfo_CriticalBk      = 0xFFFFE5E5
            .embInfo_CriticalText    = 0xFF8F0000
            .embInfo_InformationBk   = 0xFFE8F0FF
            .embInfo_InformationText = 0xFF003A8C
            .embInfo_MessageBk       = 0xFFFFF6DD
            .embInfo_MessageText     = Gray1

            .prop_FocusBk     = Color.getDarker(Color.fromArgb(Gray8), 0.04)
            .prop_FocusBorder = Gray6
            .prop_HotBk       = Color.getDarker(Color.fromArgb(Gray8), 0.02)
            .prop_HotBorder   = Transparent
            .prop_GridBorder  = Gray7
            .prop_GridColumn  = Gray7
            .prop_SectionBk   = Gray8
            .prop_CategBk     = Transparent
            .prop_Bk1         = Color.getDarker(Color.fromArgb(Gray8), 0.06)
        }
    }

    // Create a default font family with the given size
    mtd createDefaultFontfamily(size: u32)->FontFamily
    {
        var result: retval
        result.regular    = Font.create(me.res.defaultTypeFaceR, size)
        result.bold       = Font.create(me.res.defaultTypeFaceB, size)
        result.italic     = Font.create(me.res.defaultTypeFaceI, size)
        result.boldItalic = Font.create(me.res.defaultTypeFaceBI, size)
        return result
    }

    // Create a default font with the given size
    mtd createDefaultFont(size: u32, style = FontFamilyStyle.Regular)->*Font
    {
        #[Swag.Complete]
        switch style
        {
        case Regular:
            return Font.create(me.res.defaultTypeFaceR, size)
        case Bold:
            return Font.create(me.res.defaultTypeFaceB, size)
        case Italic:
            return Font.create(me.res.defaultTypeFaceI, size)
        case BoldItalic:
            return Font.create(me.res.defaultTypeFaceBI, size)
        }

        unreachable
    }

    // Create a default font (fixed size) with the given size
    mtd createDefaultFontFS(size: u32, style = FontFamilyStyle.Regular)->*Font
    {
        #[Swag.Complete]
        switch style
        {
        case Regular:
            return Font.create(me.res.defaultFsTypeFaceR, size)
        case Bold:
            return Font.create(me.res.defaultFsTypeFaceB, size)
        case Italic:
            return Font.create(me.res.defaultFsTypeFaceI, size)
        case BoldItalic:
            return Font.create(me.res.defaultFsTypeFaceBI, size)
        }

        unreachable
    }
}
