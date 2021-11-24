#global public
using Core

struct ThemeImageRect
{
    rect:   Math.Rectangle
    corner: f32
}

struct ThemeImageRects
{
    wnd_Border:             ThemeImageRect = @{{130,1,27,27}, 3}
    wnd_Caption:            ThemeImageRect = @{{57,1,72,27}, 12}
    wnd_CaptionSmall:       ThemeImageRect = @{{57,1,72,27}, 1}

    dlg_BtnBar:             ThemeImageRect = @{{29,29,27,27}, 2}

    btnPush_Normal:         ThemeImageRect = @{{1,1,27,27}, 13}
    btnPush_NormalBorder:   ThemeImageRect = @{{29,1,27,27}, 13}
    btnPush_Hot:            ThemeImageRect = @{{1,1,27,27}, 13}
    btnOush_HotBorder:      ThemeImageRect = @{{29,1,27,27}, 13}
    btnPush_Pressed:        ThemeImageRect = @{{1,1,27,27}, 13}
    btnPush_PressedBorder:  ThemeImageRect = @{{29,1,27,27}, 13}
    btnPush_Disabled:       ThemeImageRect = @{{1,1,27,27}, 13}
    btnPush_DisabledBorder: ThemeImageRect = @{{29,1,27,27}, 13}

    btnIcon_Normal:         ThemeImageRect = @{{1,29,27,27}, 9}
    btnIcon_Hot:            ThemeImageRect = @{{1,29,27,27}, 9}
    btnIcon_Pressed:        ThemeImageRect = @{{1,29,27,27}, 9}
    btnIcon_Disabled:       ThemeImageRect = @{{1,29,27,27}, 9}

    btnCheck_Normal:            ThemeImageRect = @{{90,29,32,32}, 9}
    btnCheck_NormalBorder:      ThemeImageRect = @{{57,29,32,32}, 9}
    btnCheck_Undefined:         ThemeImageRect = @{{90,29,32,32}, 9}
    btnCheck_UndefinedBorder:   ThemeImageRect = @{{57,29,32,32}, 9}
    btnCheck_UndefinedMark:     ThemeImageRect = @{{90,62,32,32}, 9}
    btnCheck_Checked:           ThemeImageRect = @{{90,29,32,32}, 9}
    btnCheck_CheckedBorder:     ThemeImageRect = @{{57,29,32,32}, 9}
    btnCheck_CheckedMark:       ThemeImageRect = @{{57,62,32,32}, 9}

    btnRadio_Normal:            ThemeImageRect = @{{156,29,32,32}, 16}
    btnRadio_NormalBorder:      ThemeImageRect = @{{123,29,32,32}, 16}
    btnRadio_Checked:           ThemeImageRect = @{{156,29,32,32}, 16}
    btnRadio_CheckedBorder:     ThemeImageRect = @{{123,29,32,32}, 16}
    btnRadio_CheckedMark:       ThemeImageRect = @{{123,62,32,32}, 16}

    edit_SquareBk:              ThemeImageRect = @{{1,57,27,27}, 5}
    edit_SquareBorder:          ThemeImageRect = @{{29,57,27,27}, 5}
    edit_RoundBk:               ThemeImageRect = @{{1,85,27,27}, 13}
    edit_RoundBorder:           ThemeImageRect = @{{29,85,27,27}, 13}
}
