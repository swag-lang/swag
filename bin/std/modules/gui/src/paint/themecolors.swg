#global public
using Pixel

struct ThemeColors
{
    richColor1:                           Color
    richColor2:                           Color
    richColor3:                           Color

    surfaceDisabled:                      Color

    hilight:                              Color
    hilightLight:                         Color
    hilightDark:                          Color
    disabled:                             Color
    transparent:                          Color
    lighterSmall:                         Color
    lighterMedium:                        Color
    lighterBig:                           Color
    lighterVeryBig:                       Color

    wnd_Bk:                               Color
    wnd_Caption:                          Color
    wnd_CaptionNotActived:                Color
    wnd_CaptionText:                      Color
    wnd_CaptionNotActivatedText:          Color
    wnd_BtnCloseBk:                       Color
    wnd_BtnCloseHotBk:                    Color
    wnd_Shadow:                           Color

    view_Bk:                              Color

    dlg_Bk:                               Color
    dlg_BtnBarBk:                         Color

    btnPush_StrongText:                   Color
    btnPush_StrongBorder:                 Color
    btnPush_StrongBk:                     Color

    btnPush_Text:                         Color
    btnPush_Border:                       Color
    btnPush_Bk:                           Color
    btnPush_PressedText:                  Color
    btnPush_PressedBorder:                Color
    btnPush_PressedBk:                    Color
    btnPush_HotText:                      Color
    btnPush_HotBorder:                    Color
    btnPush_HotBk:                        Color
    btnPush_DisabledText:                 Color
    btnPush_DisabledBorder:               Color
    btnPush_DisabledBk:                   Color

    btnPushFlat_DisabledText:             Color
    btnPushFlat_DisabledBorder:           Color
    btnPushFlat_DisabledBk:               Color
    btnPushFlat_Text:                     Color
    btnPushFlat_Border:                   Color
    btnPushFlat_Bk:                       Color
    btnPushFlat_PressedText:              Color
    btnPushFlat_PressedBorder:            Color
    btnPushFlat_PressedBk:                Color
    btnPushFlat_HotText:                  Color
    btnPushFlat_HotBorder:                Color
    btnPushFlat_HotBk:                    Color

    btnPushDefault_DisabledText:          Color
    btnPushDefault_DisabledBorder:        Color
    btnPushDefault_DisabledBk:            Color
    btnPushDefault_Text:                  Color
    btnPushDefault_Border:                Color
    btnPushDefault_Bk:                    Color
    btnPushDefault_PressedText:           Color
    btnPushDefault_PressedBorder:         Color
    btnPushDefault_PressedBk:             Color
    btnPushDefault_HotText:               Color
    btnPushDefault_HotBorder:             Color
    btnPushDefault_HotBk:                 Color

    btnIcon_Icon:                         Color
    btnIcon_Text:                         Color
    btnIcon_Bk:                           Color
    btnIcon_DisabledIcon:                 Color
    btnIcon_DisabledText:                 Color
    btnIcon_DisabledBk:                   Color
    btnIcon_PressedIcon:                  Color
    btnIcon_PressedText:                  Color
    btnIcon_PressedBk:                    Color
    btnIcon_HotIcon:                      Color
    btnIcon_HotText:                      Color
    btnIcon_HotBk:                        Color

    btnIcon_CheckedDisabledIcon:          Color
    btnIcon_CheckedDisabledText:          Color
    btnIcon_CheckedPressedIcon:           Color
    btnIcon_CheckedPressedText:           Color
    btnIcon_CheckedHotIcon:               Color
    btnIcon_CheckedHotText:               Color
    btnIcon_CheckedIcon:                  Color
    btnIcon_CheckedText:                  Color
    btnIcon_CheckedDisabledBk:            Color
    btnIcon_CheckedPressedBk:             Color
    btnIcon_CheckedHotBk:                 Color
    btnIcon_CheckedBk:                    Color
    btnIcon_CheckedMark:                  Color
    btnIcon_CheckedPressedMark:           Color
    btnIcon_CheckedHotMark:               Color
    btnIcon_CheckedDisabledMark:          Color

    label_Text:                           Color
    label_TextLight:                      Color
    label_Text1:                          Color
    label_Text2:                          Color
    label_Text3:                          Color
    label_DisabledText:                   Color
    label_Icon:                           Color
    label_DisabledIcon:                   Color

    btnCheck_Text:                        Color
    btnCheck_Border:                      Color
    btnCheck_Mark:                        Color
    btnCheck_Bk:                          Color
    btnCheck_PressedText:                 Color
    btnCheck_PressedBorder:               Color
    btnCheck_PressedMark:                 Color
    btnCheck_PressedBk:                   Color
    btnCheck_HotText:                     Color
    btnCheck_HotBorder:                   Color
    btnCheck_HotMark:                     Color
    btnCheck_HotBk:                       Color
    btnCheck_DisabledText:                Color
    btnCheck_DisabledBorder:              Color
    btnCheck_DisabledMark:                Color
    btnCheck_DisabledBk:                  Color
    btnCheck_CheckedText:                 Color
    btnCheck_CheckedBk:                   Color
    btnCheck_CheckedBorder:               Color
    btnCheck_CheckedMark:                 Color

    toolTip_Bk:                           Color
    toolTip_Border:                       Color
    toolTip_Text:                         Color

    url_Text:                             Color
    url_HotText:                          Color

    edit_Border:                          Color
    edit_FocusBorder:                     Color
    edit_HotBorder:                       Color
    edit_DisabledBorder:                  Color
    edit_Bk:                              Color
    edit_DisabledBk:                      Color
    edit_ErrorBk:                         Color
    edit_HotBk:                           Color
    edit_FocusBk:                         Color
    edit_Note:                            Color
    edit_Text:                            Color
    edit_TextUnit:                        Color
    edit_DisabledText:                    Color
    edit_SelectedText:                    Color
    edit_SelectedBk:                      Color
    edit_ErrorBorder:                     Color

    richEdit_Bk:                          Color
    richEdit_TextSpecial:                 Color
    richEdit_Text:                        Color
    richEdit_SelText:                     Color
    richEdit_SelTextNoFocus:              Color
    richEdit_SelBk:                       Color
    richEdit_SelBkNoFocus:                Color
    richEdit_Caret:                       Color
    richEdit_CurLineBorder:               Color

    btnToggle_DisabledBorder:             Color
    btnToggle_DisabledMark:               Color
    btnToggle_DisabledBk:                 Color
    btnToggle_OffBorder:                  Color
    btnToggle_OffMark:                    Color
    btnToggle_OffBk:                      Color
    btnToggle_OffHotBorder:               Color
    btnToggle_OffHotMark:                 Color
    btnToggle_OffHotBk:                   Color
    btnToggle_OffPressedBorder:           Color
    btnToggle_OffPressedMark:             Color
    btnToggle_OffPressedBk:               Color
    btnToggle_OnBorder:                   Color
    btnToggle_OnMark:                     Color
    btnToggle_OnBk:                       Color
    btnToggle_OnHotBorder:                Color
    btnToggle_OnHotMark:                  Color
    btnToggle_OnHotBk:                    Color
    btnToggle_OnPressedBorder:            Color
    btnToggle_OnPressedMark:              Color
    btnToggle_OnPressedBk:                Color

    slider_BkLeft:                        Color
    slider_BkMiddle:                      Color
    slider_BkRight:                       Color
    slider_HilightBk:                     Color
    slider_Mark:                          Color
    slider_HotMark:                       Color
    slider_PressedMark:                   Color
    slider_DisabledMark:                  Color
    slider_DisabledBk:                    Color
    slider_StepBk:                        Color
    slider_HilightStepBk:                 Color
    slider_HotStepBk:                     Color

    circular_Bk:                          Color
    circular_DisabledBk:                  Color
    circular_Circle:                      Color
    circular_DisabledCircle:              Color
    circular_Mark:                        Color
    circular_HotMark:                     Color
    circular_PressedMark:                 Color
    circular_DisabledMark:                Color
    circular_Step:                        Color
    circular_DisabledStep:                Color
    circular_RangeArc:                    Color

    tabItem_DisabledText:                 Color
    tabItem_SelectedText:                 Color
    tabItem_UnSelectedText:               Color
    tabItem_FlatSelected:                 Color
    tabItem_FlatHot:                      Color
    tabItem_RoundSelected:                Color
    tabItem_RoundHot:                     Color
    tabItem_ButtonSelected:               Color
    tabItem_ButtonHot:                    Color
    tabItem_PushButtonSelected:           Color
    tabItem_PushButtonSelectedBorder:     Color
    tabItem_PushButtonHot:                Color
    tabItem_PushButtonHotBorder:          Color
    tabItem_PushButtonBk:                 Color
    tabItem_PushButtonBkBorder:           Color
    tabBar_FlatBk:                        Color
    tabBar_RoundBk:                       Color
    tabView_FlatBk:                       Color
    tabView_RoundBk:                      Color

    menuPopup_Bk:                         Color
    menuPopup_Border:                     Color
    menuPopup_Shadow:                     Color
    menuPopup_Separator:                  Color
    menuBar_Bk:                           Color
    menuBar_Border:                       Color
    menuBar_SelectedBk:                   Color

    popupItem_Text:                       Color
    popupItem_RightText:                  Color
    popupItem_DisabledText:               Color
    popupItem_HotText:                    Color
    popupItem_HotRightText:               Color
    popupItem_HotBk:                      Color

    scrollBar_Bk:                         Color
    scrollBar_HotBk:                      Color
    scrollBar_DisabledBk:                 Color
    scrollBar_Box:                        Color
    scrollBar_HotBox:                     Color
    scrollBar_PressedBox:                 Color

    comboBox_Border:                      Color
    comboBox_HotBorder:                   Color
    comboBox_DisabledBorder:              Color
    comboBox_SelectedBorder:              Color
    comboBox_Bk:                          Color
    comboBox_HotBk:                       Color
    comboBox_DisabledBk:                  Color
    comboBox_SelectedBk:                  Color
    comboBox_Text:                        Color
    comboBox_HotText:                     Color
    comboBox_DisabledText:                Color
    comboBox_SelectedText:                Color
    comboBoxItem_Text:                    Color
    comboBoxItem_HotBk:                   Color
    comboBoxItem_HotText:                 Color
    comboBoxItem_CheckMark:               Color
    comboBoxItem_HotCheckMark:            Color

    popupList_Separator:                  Color
    popupList_HotBk:                      Color
    popupList_Text:                       Color
    popupList_HotText:                    Color
    popupList_CheckMark:                  Color
    popupList_HotCheckMark:               Color

    frameWnd_SmallShadow:                 Color
    frameWnd_Border:                      Color
    frameWnd_PopupBorder:                 Color
    frameWnd_FocusBorder:                 Color
    frameWnd_DisabledBorder:              Color
    frameWnd_Bk:                          Color
    frameWnd_DisabledBk:                  Color
    frameWnd_FocusBk:                     Color

    listItem_FocusSelectedBk:             Color
    listItem_SelectedBk:                  Color
    listItem_HotBk:                       Color
    listItem_DisabledText:                Color
    listItem_SelectedText:                Color
    listItem_Text:                        Color
    listItem_Separator:                   Color
    listItem_AlternateBk:                 Color
    listItem_Line:                        Color

    headerItem_DisabledText:              Color
    headerItem_HotText:                   Color
    headerItem_HotBk:                     Color
    headerItem_ClickedBk:                 Color
    headerItem_Text:                      Color
    headerItem_Separator:                 Color
    header_Bk:                            Color

    splitView_TitleBk:                    Color
    splitView_Bar:                        Color
    splitView_BarHot:                     Color

    progressBar_DisabledBorder:           Color
    progressBar_DisabledBk:               Color
    progressBar_DisabledMark:             Color
    progressBar_Border:                   Color
    progressBar_Bk:                       Color
    progressBar_Mark:                     Color

    palPicker_Border:                     Color
    palPicker_HotBorder:                  Color

    colorPicker_Border:                   Color
    colorPicker_BackAlpha:                Color
    colorPicker_Mark:                     Color

    imageRect_Fg:                         Color

    embInfo_CriticalBk:                   Color
    embInfo_CriticalText:                 Color
    embInfo_InformationBk:                Color
    embInfo_InformationText:              Color
    embInfo_MessageBk:                    Color
    embInfo_MessageText:                  Color

    prop_FocusBk:                         Color
    prop_FocusBorder:                     Color
    prop_HotBk:                           Color
    prop_HotBorder:                       Color
    prop_GridBorder:                      Color
    prop_GridColumn:                      Color
    prop_SectionBk:                       Color
    prop_CategBk:                         Color
    prop_Bk1:                             Color
}
