#global public
using Core

struct ThemeMetrics
{
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
    editBox_Width:              f32 = 100
    editBox_Height:             f32 = 32
}
