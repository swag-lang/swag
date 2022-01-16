#global public
using Core

struct ThemeMetrics
{
    defaultFontSize:            u32 = 14
    surfaceWnd_ShadowSize:      f32 = 20

    wnd_BorderSize:             f32 = 3
    wnd_BigCaptionCY:           f32 = 30
    wnd_SmallCaptionCY:         f32 = 24
    wnd_CaptionMarginTitle:     f32 = 5

    btnPush_Padding:            f32 = 5
    btnPush_Width:              f32 = 100
    btnPush_Height:             f32 = 30

    iconText_Margin:            f32 = 4

    btnCheck_Size:              f32 = 24
    btnCheck_Padding:           f32 = 4

    msgDlg_Padding:             f32 = 20
    msgDlg_BtnPadding:          f32 = 5

    editBox_SquarePadding:      Math.Vector4 = @{10, 5, 10, 5}
    editBox_RoundPadding:       Math.Vector4 = @{12, 5, 12, 5}
    editBox_FlatPadding:        Math.Vector4 = @{12, 0, 12, 5}
    editBox_Width:              f32 = 100
    editBox_Height:             f32 = 32

    btnToggle_Width:            f32 = 50
    btnToggle_Height:           f32 = 26

    slider_Width:               f32 = 200
    slider_Height:              f32 = 26

    menuPopup_Padding:          f32 = 10
    menuPopup_SpaceLines:       f32 = 6
    menuPopup_ShadowSize:       f32 = 10
    menuPopup_IconTextMargin:   f32 = 6
    menuPopup_SeparatorHeight:  f32 = 8

    menuBar_Height:             f32 = 30
    menuBar_SpaceBetween:       f32 = 24

    scrollBar_NormalSize:       f32 = 20
    scrollBar_SmallSize:        f32 = 10
    scrollBar_BoxPadding:       f32 = 4
    scrollBar_BoxMinSize:       f32 = 20

    comboBox_Width:             f32 = 150
    comboBox_BorderSize:        f32 = 1
    comboBox_ShadowSize:        f32 = 15
    comboBox_Height:            f32 = 32
    comboBox_RightMargin:       f32 = 30
    comboBox_Padding:           f32 = 6
    comboBox_IconTextMargin:    f32 = 6

    frameWnd_SquarePadding:     Math.Vector4 = @{5, 5, 5, 5}
    frameWnd_RoundPadding:      Math.Vector4 = @{5, 5, 5, 5}
    frameWnd_FlatPadding:       Math.Vector4 = @{2, 2, 2, 2}
}
