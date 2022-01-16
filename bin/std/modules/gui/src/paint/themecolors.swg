#global public
using Pixel

struct ThemeColors
{
    const Blue          = 0xFF1473e6
    const LightBlue     = 0xFF2483f6
    const Disabled      = 0x5FFFFFFF
    const LighterSmall  = 0x1FFFFFFF
    const LighterMedium = 0x3FFFFFFF
    const LighterBig    = 0x7FFFFFFF
    const Transparent   = 0x00000000

    const White         = 0xFFF0F0F0
    const Gray1         = 0xFFE0E0E0
    const Gray2         = 0xFF808080
    const Gray3         = 0xFF606060
    const Gray4         = 0xFF505050
    const Gray5         = 0xFF404040
    const Gray6         = 0xFF353535
    const Gray7         = 0xFF303030

    richColor1: Color = Argb.Red
    richColor2: Color = Argb.Green
    richColor3: Color = Argb.Blue

    wnd_Bk:                         Color = Gray5
    wnd_Caption:                    Color = Gray4
    wnd_CaptionNotActived:          Color = Gray4
    wnd_CaptionText:                Color = White
    wnd_CaptionNotActivatedText:    Color = White
    wnd_BtnCloseBk:                 Color = 0xFFd61425
    wnd_BtnCloseHotBk:              Color = 0xFFe62435
    wnd_Shadow:                     Color = 0x8C000000

    dlg_Bk:                         Color = Gray6
    dlg_BtnBarBk:                   Color = Gray7

    btnPush_Text:                   Color = White
    btnPush_Border:                 Color = White
    btnPush_Bk:                     Color = Transparent
    btnPush_PressedText:            Color = Gray5
    btnPush_PressedBorder:          Color = White
    btnPush_PressedBk:              Color = White
    btnPush_HotText:                Color = Gray5
    btnPush_HotBorder:              Color = Gray1
    btnPush_HotBk:                  Color = Gray1
    btnPush_DisabledText:           Color = Disabled
    btnPush_DisabledBorder:         Color = Disabled
    btnPush_DisabledBk:             Color = Transparent

    btnPushFlat_DisabledText:       Color = Disabled
    btnPushFlat_DisabledBorder:     Color = Disabled
    btnPushFlat_DisabledBk:         Color = Transparent
    btnPushFlat_Text:               Color = White
    btnPushFlat_Border:             Color = Transparent
    btnPushFlat_Bk:                 Color = Transparent
    btnPushFlat_PressedText:        Color = White
    btnPushFlat_PressedBorder:      Color = Transparent
    btnPushFlat_PressedBk:          Color = Gray3
    btnPushFlat_HotText:            Color = White
    btnPushFlat_HotBorder:          Color = Transparent
    btnPushFlat_HotBk:              Color = Gray4

    btnPushDefault_DisabledText:    Color = Disabled
    btnPushDefault_DisabledBorder:  Color = Disabled
    btnPushDefault_DisabledBk:      Color = Transparent
    btnPushDefault_Text:            Color = White
    btnPushDefault_Border:          Color = Blue
    btnPushDefault_Bk:              Color = Blue
    btnPushDefault_PressedText:     Color = White
    btnPushDefault_PressedBorder:   Color = Blue
    btnPushDefault_PressedBk:       Color = Blue
    btnPushDefault_HotText:         Color = White
    btnPushDefault_HotBorder:       Color = LightBlue
    btnPushDefault_HotBk:           Color = LightBlue

    btnIcon_Icon:                   Color = White
    btnIcon_Bk:                     Color = Transparent
    btnIcon_DisabledIcon:           Color = White
    btnIcon_DisabledBk:             Color = Transparent
    btnIcon_PressedIcon:            Color = White
    btnIcon_PressedBk:              Color = LighterBig
    btnIcon_HotIcon:                Color = White
    btnIcon_HotBk:                  Color = LighterMedium

    label_Text:                     Color = White
    label_Text1:                    Color = 0xFFFF5555
    label_Text2:                    Color = 0xFF55FF55
    label_Text3:                    Color = 0xFFFF5555
    label_DisabledText:             Color = Disabled
    label_Icon:                     Color = White
    label_DisabledIcon:             Color = White

    btnCheck_Text:                  Color = White
    btnCheck_Border:                Color = White
    btnCheck_Mark:                  Color = White
    btnCheck_Bk:                    Color = Transparent
    btnCheck_PressedText:           Color = White
    btnCheck_PressedBorder:         Color = White
    btnCheck_PressedMark:           Color = White
    btnCheck_PressedBk:             Color = LighterBig
    btnCheck_HotText:               Color = White
    btnCheck_HotBorder:             Color = White
    btnCheck_HotMark:               Color = White
    btnCheck_HotBk:                 Color = LighterSmall
    btnCheck_DisabledText:          Color = Disabled
    btnCheck_DisabledBorder:        Color = Disabled
    btnCheck_DisabledMark:          Color = Disabled
    btnCheck_DisabledBk:            Color = Transparent

    edit_Border:                    Color = Gray3
    edit_FocusBorder:               Color = Blue
    edit_HotBorder:                 Color = Gray3
    edit_DisabledBorder:            Color = Gray3
    edit_Bk:                        Color = Gray6
    edit_DisabledBk:                Color = Gray5
    edit_ErrorBk:                   Color = Gray6
    edit_HotBk:                     Color = Gray6
    edit_FocusBk:                   Color = Gray6
    edit_Note:                      Color = Disabled
    edit_Text:                      Color = White
    edit_DisabledText:              Color = Disabled
    edit_SelectedText:              Color = White
    edit_SelectedBk:                Color = Blue
    edit_ErrorBorder:               Color = 0xFFFF0000

    btnToggle_DisabledBorder:       Color = Disabled
    btnToggle_DisabledMark:         Color = Disabled
    btnToggle_DisabledBk:           Color = Transparent
    btnToggle_OffBorder:            Color = White
    btnToggle_OffMark:              Color = White
    btnToggle_OffBk:                Color = Transparent
    btnToggle_OffHotBorder:         Color = White
    btnToggle_OffHotMark:           Color = White
    btnToggle_OffHotBk:             Color = LighterSmall
    btnToggle_OffPressedBorder:     Color = White
    btnToggle_OffPressedMark:       Color = White
    btnToggle_OffPressedBk:         Color = LighterBig
    btnToggle_OnBorder:             Color = White
    btnToggle_OnMark:               Color = White
    btnToggle_OnBk:                 Color = Blue
    btnToggle_OnHotBorder:          Color = White
    btnToggle_OnHotMark:            Color = White
    btnToggle_OnHotBk:              Color = LightBlue
    btnToggle_OnPressedBorder:      Color = White
    btnToggle_OnPressedMark:        Color = White
    btnToggle_OnPressedBk:          Color = Blue

    slider_BkLeft:                  Color = Gray6
    slider_BkMiddle:                Color = Gray6
    slider_BkRight:                 Color = Gray6
    slider_HilightBk:               Color = Blue
    slider_Mark:                    Color = Blue
    slider_HotMark:                 Color = White
    slider_PressedMark:             Color = White
    slider_DisabledMark:            Color = Gray4
    slider_DisabledBk:              Color = Gray6
    slider_StepBk:                  Color = Color.getLighter(Color.fromArgb(Gray6), 0.1)
    slider_HilightStepBk:           Color = Color.getDarker(Color.fromArgb(Blue), 0.1)
    slider_HotStepBk:               Color = Color.getDarker(Color.fromArgb(White), 0.3)

    tabItem_DisabledText:           Color = Gray4
    tabItem_SelectedText:           Color = White
    tabItem_UnSelectedText:         Color = Color.getDarker(Color.fromArgb(White), 0.3)
    tabItem_FlatSelected:           Color = Blue
    tabItem_FlatHot:                Color = White
    tabItem_RoundSelected:          Color = Gray6
    tabItem_RoundHot:               Color = Gray4
    tabBar_FlatBk:                  Color = Gray4
    tabView_FlatBk:                 Color = Gray6
    tabView_RoundBk:                Color = Gray6

    menuPopup_Bk:                   Color = Gray6
    menuPopup_Border:               Color = 0
    menuPopup_Shadow:               Color = 0xFF000000
    menuPopup_Separator:            Color = Color.getLighter(Color.fromArgb(Gray6), 0.2)
    menuBar_Bk:                     Color = Gray4
    menuBar_Border:                 Color = 0
    menuBar_SelectedBk:             Color = Gray5

    popupItem_Text:                 Color = Gray1
    popupItem_RightText:            Color = Gray2
    popupItem_DisabledText:         Color = Gray2
    popupItem_HotText:              Color = White
    popupItem_HotRightText:         Color = White
    popupItem_HotBk:                Color = Blue

    scrollBar_Bk:                   Color = Gray6
    scrollBar_Box:                  Color = Gray4
    scrollBar_HotBox:               Color = Gray2
    scrollBar_PressedBox:           Color = Gray1

    comboBox_Shadow:                Color = 0x80000000
    comboBox_Border:                Color = Gray3
    comboBox_HotBorder:             Color = Gray2
    comboBox_DisabledBorder:        Color = Gray3
    comboBox_SelectedBorder:        Color = Gray2
    comboBox_Bk:                    Color = Color.getDarker(Color.fromArgb(Gray5), 0.05)
    comboBox_HotBk:                 Color = Gray6
    comboBox_DisabledBk:            Color = Gray5
    comboBox_SelectedBk:            Color = Gray6
    comboBox_Text:                  Color = Gray1
    comboBox_HotText:               Color = White
    comboBox_DisabledText:          Color = Gray3
    comboBox_SelectedText:          Color = White
    comboBoxItem_Text:              Color = Gray1
    comboBoxItem_HotBk:             Color = Blue
    comboBoxItem_HotText:           Color = White
    comboBoxItem_CheckMark:         Color = Blue
    comboBoxItem_HotCheckMark:      Color = White

    list_Border:                    Color = Gray3
    list_FocusBorder:               Color = Blue
    list_DisabledBorder:            Color = Gray3
    list_Bk:                        Color = Gray6
    list_DisabledBk:                Color = Gray5
    list_FocusBk:                   Color = Gray6
}
