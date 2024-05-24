#global public
using Pixel

struct ThemeColors
{
    const Transparent                  = 0x00000000
    const Blue                         = 0xFF1473e6
    const LightBlue                    = 0xFF2483f6
    const Disabled                     = 0x5FFFFFFF
    const LighterSmall                 = 0x0FFFFFFF
    const LighterMedium                = 0x1FFFFFFF
    const LighterBig                   = 0x2FFFFFFF
    const LighterVeryBig               = 0x3FFFFFFF
    const White                        = 0xFFF0F0F0
    const Gray1                        = 0xFFE0E0E0
    const Gray2                        = 0xFF808080
    const Gray3                        = 0xFF606060
    const Gray4                        = 0xFF505050
    const Gray5                        = 0xFF404040
    const Gray6                        = 0xFF353535
    const Gray7                        = 0xFF303030
    const Gray8                        = 0xFF202020

    richColor1:                        Color = Argb.Red
    richColor2:                        Color = Argb.Green
    richColor3:                        Color = Argb.Blue

    surfaceDisabled:                   Color = Color.fromArgb(0xFFBBBBBB)

    hilight:                           Color = Blue
    hilightLight:                      Color = Color.getLighter(Color.fromArgb(Blue), 0.2)
    hilightDark:                       Color = Color.getDarker(Color.fromArgb(Blue), 0.2)
    disabled:                          Color = Disabled
    transparent:                       Color = Transparent
    lighterSmall:                      Color = LighterSmall
    lighterMedium:                     Color = LighterMedium
    lighterBig:                        Color = LighterBig
    lighterVeryBig:                    Color = LighterVeryBig

    wnd_Bk:                            Color = Gray5
    wnd_Caption:                       Color = Gray4
    wnd_CaptionNotActived:             Color = Gray4
    wnd_CaptionText:                   Color = White
    wnd_CaptionNotActivatedText:       Color = White
    wnd_BtnCloseBk:                    Color = 0xFFd61425
    wnd_BtnCloseHotBk:                 Color = 0xFFe62435
    wnd_Shadow:                        Color = 0x8C000000

    view_Bk:                           Color = 0xFF282828

    dlg_Bk:                            Color = Gray6
    dlg_BtnBarBk:                      Color = Gray7

    btnPush_StrongText:                Color = White
    btnPush_StrongBorder:              Color = White
    btnPush_StrongBk:                  Color = Transparent

    btnPush_Text:                      Color = White
    btnPush_Border:                    Color = Gray2
    btnPush_Bk:                        Color = Transparent
    btnPush_PressedText:               Color = Gray5
    btnPush_PressedBorder:             Color = White
    btnPush_PressedBk:                 Color = White
    btnPush_HotText:                   Color = Gray5
    btnPush_HotBorder:                 Color = Gray1
    btnPush_HotBk:                     Color = Gray1
    btnPush_DisabledText:              Color = Disabled
    btnPush_DisabledBorder:            Color = Disabled
    btnPush_DisabledBk:                Color = Transparent

    btnPushFlat_DisabledText:          Color = Disabled
    btnPushFlat_DisabledBorder:        Color = Disabled
    btnPushFlat_DisabledBk:            Color = Transparent
    btnPushFlat_Text:                  Color = White
    btnPushFlat_Border:                Color = Transparent
    btnPushFlat_Bk:                    Color = Transparent
    btnPushFlat_PressedText:           Color = White
    btnPushFlat_PressedBorder:         Color = Transparent
    btnPushFlat_PressedBk:             Color = Gray3
    btnPushFlat_HotText:               Color = White
    btnPushFlat_HotBorder:             Color = Transparent
    btnPushFlat_HotBk:                 Color = Gray4

    btnPushDefault_DisabledText:       Color = Disabled
    btnPushDefault_DisabledBorder:     Color = Disabled
    btnPushDefault_DisabledBk:         Color = Transparent
    btnPushDefault_Text:               Color = White
    btnPushDefault_Border:             Color = Blue
    btnPushDefault_Bk:                 Color = Blue
    btnPushDefault_PressedText:        Color = White
    btnPushDefault_PressedBorder:      Color = Blue
    btnPushDefault_PressedBk:          Color = Blue
    btnPushDefault_HotText:            Color = White
    btnPushDefault_HotBorder:          Color = LightBlue
    btnPushDefault_HotBk:              Color = LightBlue

    btnIcon_Icon:                      Color = White
    btnIcon_Text:                      Color = White
    btnIcon_Bk:                        Color = Transparent
    btnIcon_DisabledIcon:              Color = Disabled
    btnIcon_DisabledText:              Color = Disabled
    btnIcon_DisabledBk:                Color = Transparent
    btnIcon_PressedIcon:               Color = White
    btnIcon_PressedText:               Color = White
    btnIcon_PressedBk:                 Color = LighterBig
    btnIcon_HotIcon:                   Color = White
    btnIcon_HotText:                   Color = White
    btnIcon_HotBk:                     Color = LighterMedium

    btnIcon_CheckedDisabledIcon:       Color = Disabled
    btnIcon_CheckedDisabledText:       Color = Disabled
    btnIcon_CheckedPressedIcon:        Color = White
    btnIcon_CheckedPressedText:        Color = White
    btnIcon_CheckedHotIcon:            Color = White
    btnIcon_CheckedHotText:            Color = White
    btnIcon_CheckedIcon:               Color = White
    btnIcon_CheckedText:               Color = White
    btnIcon_CheckedDisabledBk:         Color = Transparent
    btnIcon_CheckedPressedBk:          Color = LighterBig
    btnIcon_CheckedHotBk:              Color = Transparent
    btnIcon_CheckedBk:                 Color = Transparent
    btnIcon_CheckedMark:               Color = Color.getLighter(Color.fromArgb(Blue), 0.2)
    btnIcon_CheckedPressedMark:        Color = Blue
    btnIcon_CheckedHotMark:            Color = Color.getLighter(Color.fromArgb(Blue), 0.1)
    btnIcon_CheckedDisabledMark:       Color = Disabled

    label_Text:                        Color = Gray1
    label_TextLight:                   Color = Gray2
    label_Text1:                       Color = 0xFFFF5555
    label_Text2:                       Color = 0xFF55FF55
    label_Text3:                       Color = 0xFFFF5555
    label_DisabledText:                Color = Disabled
    label_Icon:                        Color = White
    label_DisabledIcon:                Color = White

    btnCheck_Text:                     Color = White
    btnCheck_Border:                   Color = White
    btnCheck_Mark:                     Color = White
    btnCheck_Bk:                       Color = Transparent
    btnCheck_PressedText:              Color = White
    btnCheck_PressedBorder:            Color = White
    btnCheck_PressedMark:              Color = White
    btnCheck_PressedBk:                Color = LighterBig
    btnCheck_HotText:                  Color = White
    btnCheck_HotBorder:                Color = White
    btnCheck_HotMark:                  Color = White
    btnCheck_HotBk:                    Color = LighterSmall
    btnCheck_DisabledText:             Color = Disabled
    btnCheck_DisabledBorder:           Color = Disabled
    btnCheck_DisabledMark:             Color = Disabled
    btnCheck_DisabledBk:               Color = Transparent
    btnCheck_CheckedText:              Color = White
    btnCheck_CheckedBk:                Color = Transparent
    btnCheck_CheckedBorder:            Color = White
    btnCheck_CheckedMark:              Color = White

    toolTip_Bk:                        Color = 0xFFCCCC88
    toolTip_Border:                    Color = 0xFF000000
    toolTip_Text:                      Color = 0xFF222222

    url_Text:                          Color = Blue
    url_HotText:                       Color = Color.getLighter(Color.fromArgb(Blue), 0.2)

    edit_Border:                       Color = Gray3
    edit_FocusBorder:                  Color = Blue
    edit_HotBorder:                    Color = Gray3
    edit_DisabledBorder:               Color = Gray3
    edit_Bk:                           Color = Gray6
    edit_DisabledBk:                   Color = Gray5
    edit_ErrorBk:                      Color = Gray6
    edit_HotBk:                        Color = Gray6
    edit_FocusBk:                      Color = Gray6
    edit_Note:                         Color = Disabled
    edit_Text:                         Color = White
    edit_TextUnit:                     Color = Disabled
    edit_DisabledText:                 Color = Disabled
    edit_SelectedText:                 Color = White
    edit_SelectedBk:                   Color = Blue
    edit_ErrorBorder:                  Color = 0xFFFF0000

    richEdit_Bk:                       Color = Gray8
    richEdit_TextSpecial:              Color = Gray7
    richEdit_Text:                     Color = White
    richEdit_SelText:                  Color = Transparent
    richEdit_SelTextNoFocus:           Color = Transparent
    richEdit_SelBk:                    Color = Color.getDarker(Color.fromArgb(Blue), 0.5)
    richEdit_SelBkNoFocus:             Color = Gray6
    richEdit_Caret:                    Color = White
    richEdit_CurLineBorder:            Color = Gray4

    btnToggle_DisabledBorder:          Color = Disabled
    btnToggle_DisabledMark:            Color = Disabled
    btnToggle_DisabledBk:              Color = Transparent
    btnToggle_OffBorder:               Color = White
    btnToggle_OffMark:                 Color = White
    btnToggle_OffBk:                   Color = Transparent
    btnToggle_OffHotBorder:            Color = White
    btnToggle_OffHotMark:              Color = White
    btnToggle_OffHotBk:                Color = LighterSmall
    btnToggle_OffPressedBorder:        Color = White
    btnToggle_OffPressedMark:          Color = White
    btnToggle_OffPressedBk:            Color = LighterBig
    btnToggle_OnBorder:                Color = White
    btnToggle_OnMark:                  Color = White
    btnToggle_OnBk:                    Color = Blue
    btnToggle_OnHotBorder:             Color = White
    btnToggle_OnHotMark:               Color = White
    btnToggle_OnHotBk:                 Color = LightBlue
    btnToggle_OnPressedBorder:         Color = White
    btnToggle_OnPressedMark:           Color = White
    btnToggle_OnPressedBk:             Color = Blue

    slider_BkLeft:                     Color = Gray6
    slider_BkMiddle:                   Color = Gray6
    slider_BkRight:                    Color = Gray6
    slider_HilightBk:                  Color = Blue
    slider_Mark:                       Color = Blue
    slider_HotMark:                    Color = White
    slider_PressedMark:                Color = White
    slider_DisabledMark:               Color = Gray4
    slider_DisabledBk:                 Color = Gray6
    slider_StepBk:                     Color = Color.getLighter(Color.fromArgb(Gray6), 0.1)
    slider_HilightStepBk:              Color = Color.getDarker(Color.fromArgb(Blue), 0.1)
    slider_HotStepBk:                  Color = Color.getDarker(Color.fromArgb(White), 0.3)

    tabItem_DisabledText:              Color = Gray4
    tabItem_SelectedText:              Color = White
    tabItem_UnSelectedText:            Color = Color.getDarker(Color.fromArgb(White), 0.3)
    tabItem_FlatSelected:              Color = Blue
    tabItem_FlatHot:                   Color = White
    tabItem_RoundSelected:             Color = Gray6
    tabItem_RoundHot:                  Color = Gray4
    tabItem_ButtonSelected:            Color = Gray4
    tabItem_ButtonHot:                 Color = Gray5
    tabItem_PushButtonSelected:        Color = Blue
    tabItem_PushButtonSelectedBorder:  Color = White
    tabItem_PushButtonHot:             Color = Gray5
    tabItem_PushButtonHotBorder:       Color = Gray2
    tabItem_PushButtonBk:              Color = Gray6
    tabItem_PushButtonBkBorder:        Color = Gray3
    tabBar_FlatBk:                     Color = Gray4
    tabBar_RoundBk:                    Color = Gray6
    tabView_FlatBk:                    Color = Gray6
    tabView_RoundBk:                   Color = Gray6

    menuPopup_Bk:                      Color = Gray6
    menuPopup_Border:                  Color = Color.getLighter(Color.fromArgb(Gray6), 0.2)
    menuPopup_Shadow:                  Color = 0xFF000000
    menuPopup_Separator:               Color = Color.getLighter(Color.fromArgb(Gray6), 0.2)
    menuBar_Bk:                        Color = Gray4
    menuBar_Border:                    Color = 0
    menuBar_SelectedBk:                Color = 0x1F000000

    popupItem_Text:                    Color = Gray1
    popupItem_RightText:               Color = Gray2
    popupItem_DisabledText:            Color = Gray2
    popupItem_HotText:                 Color = White
    popupItem_HotRightText:            Color = White
    popupItem_HotBk:                   Color = Blue

    scrollBar_Bk:                      Color = Gray6
    scrollBar_HotBk:                   Color = 0x10888888
    scrollBar_DisabledBk:              Color = 0
    scrollBar_Box:                     Color = Gray3
    scrollBar_HotBox:                  Color = Gray2
    scrollBar_PressedBox:              Color = Gray1

    comboBox_Border:                   Color = Gray3
    comboBox_HotBorder:                Color = Gray2
    comboBox_DisabledBorder:           Color = Gray3
    comboBox_SelectedBorder:           Color = Gray2
    comboBox_Bk:                       Color = Gray6
    comboBox_HotBk:                    Color = Color.getDarker(Color.fromArgb(Gray5), 0.05)
    comboBox_DisabledBk:               Color = Gray5
    comboBox_SelectedBk:               Color = Gray6
    comboBox_Text:                     Color = Gray1
    comboBox_HotText:                  Color = White
    comboBox_DisabledText:             Color = Gray3
    comboBox_SelectedText:             Color = White
    comboBoxItem_Text:                 Color = Gray1
    comboBoxItem_HotBk:                Color = Blue
    comboBoxItem_HotText:              Color = White
    comboBoxItem_CheckMark:            Color = Blue
    comboBoxItem_HotCheckMark:         Color = White

    popupList_Separator:               Color = Gray4
    popupList_HotBk:                   Color = Blue
    popupList_Text:                    Color = Gray1
    popupList_HotText:                 Color = White
    popupList_CheckMark:               Color = Blue
    popupList_HotCheckMark:            Color = White

    frameWnd_SmallShadow:              Color = 0x50000000
    frameWnd_Border:                   Color = Gray3
    frameWnd_PopupBorder:              Color = Gray2
    frameWnd_FocusBorder:              Color = Blue
    frameWnd_DisabledBorder:           Color = Gray3
    frameWnd_Bk:                       Color = Gray6
    frameWnd_DisabledBk:               Color = Gray5
    frameWnd_FocusBk:                  Color = Gray6

    listItem_FocusSelectedBk:          Color = Blue
    listItem_SelectedBk:               Color = Gray5
    listItem_HotBk:                    Color = Gray5
    listItem_DisabledText:             Color = Gray3
    listItem_SelectedText:             Color = White
    listItem_Text:                     Color = White
    listItem_Separator:                Color = Color.getDarker(Color.fromArgb(White), 0.3)
    listItem_AlternateBk:              Color = 0x08FFFFFF
    listItem_Line:                     Color = Gray4

    headerItem_DisabledText:           Color = Gray4
    headerItem_HotText:                Color = White
    headerItem_HotBk:                  Color = 0x10FFFFFF
    headerItem_ClickedBk:              Color = 0x20FFFFFF
    headerItem_Text:                   Color = White
    headerItem_Separator:              Color = Color.getLighter(Color.fromArgb(Gray4), 0.2)
    header_Bk:                         Color = Gray4

    splitView_TitleBk:                 Color = Gray6
    splitView_Bar:                     Color = Gray4
    splitView_BarHot:                  Color = Blue

    progressBar_DisabledBorder:        Color = Gray3
    progressBar_DisabledBk:            Color = Transparent
    progressBar_DisabledMark:          Color = Gray3
    progressBar_Border:                Color = Gray2
    progressBar_Bk:                    Color = Gray6
    progressBar_Mark:                  Color = Blue

    palPicker_Border:                  Color = Gray7
    palPicker_HotBorder:               Color = Blue

    colorPicker_Border:                Color = 0x3F000000
    colorPicker_BackAlpha:             Color = Gray4
    colorPicker_Mark:                  Color = Gray1

    imageRect_Fg:                      Color = White

    embInfo_CriticalBk:                Color = 0xFF8F0000
    embInfo_InformationBk:             Color = 0xFF303050

    prop_FocusBk:                      Color = Color.getLighter(Color.fromArgb(Gray5), 0.05)
    prop_FocusBorder:                  Color = Gray3
    prop_HotBk:                        Color = Color.getDarker(Color.fromArgb(Gray5), 0.03)
    prop_HotBorder:                    Color = Transparent
    prop_GridBorder:                   Color = Gray7
    prop_GridColumn:                   Color = Gray7
    prop_SectionBk:                    Color = Gray6
    prop_CategBk:                      Color = Transparent
    prop_Bk1:                          Color = Color.getDarker(Color.fromArgb(Gray5), 0.04)
}
