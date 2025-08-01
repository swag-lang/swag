#global public
using Core

struct ThemeMetrics
{
    defaultFontSize:                       u32 = 14
    defaultFixedFontSize:                  u32 = 16
    surfaceWnd_ShadowSize:                 f32 = 20

    wnd_BorderSize:                        f32 = 3
    wnd_BigCaptionCY:                      f32 = 30
    wnd_SmallCaptionCY:                    f32 = 24
    wnd_CaptionMarginTitle:                f32 = 5

    tabBar_FlatHeight:                     f32 = 40
    tabBar_RoundHeight:                    f32 = 28
    tabBar_ButtonHeight:                   f32 = 32

    btnPush_Padding:                       f32 = 5
    btnPush_Width:                         f32 = 100
    btnPush_Height:                        f32 = 30

    iconText_Margin:                       f32 = 4

    btnCheck_Size:                         f32 = 24
    btnCheck_Padding:                      f32 = 4

    dialog_Padding:                        f32 = 20
    dialog_BtnPadding:                     f32 = 5

    editBox_SquarePadding:                 Math.Vector4 = {10, 5, 10, 5}
    editBox_RoundPadding:                  Math.Vector4 = {12, 5, 12, 5}
    editBox_FlatPadding:                   Math.Vector4 = {12, 0, 12, 5}
    editBox_Width:                         f32 = 100
    editBox_Height:                        f32 = 32
    editBox_UnitMargin:                    f32 = 3

    btnToggle_Width:                       f32 = 50
    btnToggle_Height:                      f32 = 26

    slider_Width:                          f32 = 200
    slider_Height:                         f32 = 26
    slider_BoxSize:                        f32 = 18
    slider_StepSize:                       f32 = 8

    circular_Size:                         f32 = 40
    circular_MarkerSize:                   f32 = 5

    menuPopup_Padding:                     f32 = 12
    menuPopup_SpaceLines:                  f32 = 6
    menuPopup_ShadowSize:                  f32 = 10
    menuPopup_IconMargin:                  f32 = 2
    menuPopup_IconTextMargin:              f32 = 6
    menuPopup_SeparatorHeight:             f32 = 8

    menuBar_Height:                        f32 = 30
    menuBar_SpaceBetween:                  f32 = 24

    iconBar_ButtonsPadding:                f32 = 4

    scrollBar_NormalSize:                  f32 = 20
    scrollBar_SmallSize:                   f32 = 10
    scrollBar_BoxPadding:                  f32 = 4
    scrollBar_BoxMinSize:                  f32 = 20

    comboBox_Width:                        f32 = 150
    comboBox_Height:                       f32 = 32
    comboBox_RightMargin:                  f32 = 30
    comboBox_Padding:                      f32 = 10
    comboBox_IconTextMargin:               f32 = 6

    popupList_Padding:                     f32 = 6
    popupList_IconTextMargin:              f32 = 6
    popupList_SeparatorPadding:            f32 = 4

    btnIcon_WidthArrow:                    f32 = 10
    btnIcon_HeightArrow:                   f32 = 6
    btnIcon_Padding:                       f32 = 4
    btnIcon_MarginArrow:                   f32 = 4
    btnIcon_SpacingIcon:                   f32 = 4
    btnIcon_SpacingArrowH:                 f32 = 8
    btnIcon_SpacingArrowV:                 f32 = 4
    btnIcon_SpacingTextH:                  f32 = 8
    btnIcon_SpacingTextV:                  f32 = 0

    frameWnd_SquarePadding:                f32 = 5
    frameWnd_RoundPadding:                 f32 = 5
    frameWnd_FlatPadding:                  f32 = 2
    frameWnd_SmallShadowSize:              Math.Vector4{5, 5, 15, 15}
    frameWnd_AnchorWidth:                  f32 = 24
    frameWnd_AnchorHeight:                 f32 = 10
    frameWnd_AnchorOverlap:                f32 = 2
    frameWnd_AnchorRoundBorderMargin:      f32 = 16
    frameWnd_AnchorSquareBorderMargin:     f32 = 8

    list_Padding:                          f32 = 5
    list_ExpandSize:                       f32 = 22

    header_Height:                         f32 = 35
    header_Padding:                        Math.Vector4 = {10, 5, 10, 5}
    header_Separator:                      f32 = 5

    splitView_BarSize:                     f32 = 8

    progressBar_Height:                    f32 = 18
    progressBar_ClipOffset:                f32 = 2
    progressBar_SpeedInf:                  f32 = 0.5

    colorPicker_RoundPick:                 f32 = 6
    colorPicker_SquarePick:                f32 = 6

    label_Height:                          f32 = 24

    richEdit_Padding:                      f32 = 5
    toolTip_Padding:                       f32 = 2
}
