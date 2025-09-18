#global public
using Core

struct ThemeImageRect
{
    rect:                Math.Rectangle
    corner:              f32
    keepBordersRatio     = true
}

struct ThemeImageRects
{
    surfaceWnd_Shadow:              ThemeImageRect = {{189, 1, 66, 66}, 24}

    wnd_Border:                     ThemeImageRect = {{130, 1, 27, 27}, 3}
    wnd_CaptionBk:                  ThemeImageRect = {{57, 1, 72, 27}, 12}
    wnd_CaptionSmall:               ThemeImageRect = {{57, 1, 72, 27}, 1}

    dlg_BtnBar:                     ThemeImageRect = {{29, 29, 27, 27}, 2}

    btnPush_Normal:                 ThemeImageRect = {{1, 1, 27, 27}, 13}
    btnPush_NormalBorder:           ThemeImageRect = {{29, 1, 27, 27}, 13}
    btnPush_Hot:                    ThemeImageRect = {{1, 1, 27, 27}, 13}
    btnPush_HotBorder:              ThemeImageRect = {{29, 1, 27, 27}, 13}
    btnPush_Pressed:                ThemeImageRect = {{1, 1, 27, 27}, 13}
    btnPush_PressedBorder:          ThemeImageRect = {{29, 1, 27, 27}, 13}
    btnPush_Disabled:               ThemeImageRect = {{1, 1, 27, 27}, 13}
    btnPush_DisabledBorder:         ThemeImageRect = {{29, 1, 27, 27}, 13}

    btnIcon_SquareBk:               ThemeImageRect = {{1, 29, 27, 27}, 9}
    btnIcon_RoundBk:                ThemeImageRect = {{85, 141, 27, 27}, 13}
    btnIcon_RoundSquareBk:          ThemeImageRect = {{141, 225, 27, 27}, 7}

    btnIcon_CheckedBottom:          ThemeImageRect = {{1, 281, 27, 27}, 7}
    btnIcon_CheckedTop:             ThemeImageRect = {{29, 281, 27, 27}, 7}
    btnIcon_CheckedLeft:            ThemeImageRect = {{57, 281, 27, 27}, 7}
    btnIcon_CheckedRight:           ThemeImageRect = {{85, 281, 27, 27}, 7}
    btnIcon_CheckedSquare:          ThemeImageRect = {{169, 309, 27, 27}, 9}
    btnIcon_CheckedRound:           ThemeImageRect = {{197, 309, 27, 27}, 13}
    btnIcon_CheckedRoundSquare:     ThemeImageRect = {{225, 309, 27, 27}, 7}

    btnIcon_ArrowDown:              ThemeImageRect = {{1, 337, 27, 27}, 0}
    btnIcon_ArrowLeft:              ThemeImageRect = {{29, 337, 27, 27}, 0}
    btnIcon_ArrowUp:                ThemeImageRect = {{57, 337, 27, 27}, 0}
    btnIcon_ArrowRight:             ThemeImageRect = {{85, 337, 27, 27}, 0}

    btnCheck_Normal:                ThemeImageRect = {{90, 29, 32, 32}, 16}
    btnCheck_NormalBorder:          ThemeImageRect = {{57, 29, 32, 32}, 16}
    btnCheck_Undefined:             ThemeImageRect = {{90, 29, 32, 32}, 16}
    btnCheck_UndefinedBorder:       ThemeImageRect = {{57, 29, 32, 32}, 16}
    btnCheck_UndefinedMark:         ThemeImageRect = {{90, 62, 32, 32}, 16}
    btnCheck_Checked:               ThemeImageRect = {{90, 29, 32, 32}, 16}
    btnCheck_CheckedBorder:         ThemeImageRect = {{57, 29, 32, 32}, 16}
    btnCheck_CheckedMark:           ThemeImageRect = {{57, 62, 32, 32}, 0}

    btnRadio_NormalBorder:          ThemeImageRect = {{123, 29, 32, 32}, 16}
    btnRadio_Normal:                ThemeImageRect = {{156, 29, 32, 32}, 16}
    btnRadio_CheckedBorder:         ThemeImageRect = {{123, 29, 32, 32}, 16}
    btnRadio_Checked:               ThemeImageRect = {{156, 29, 32, 32}, 16}
    btnRadio_CheckedMark:           ThemeImageRect = {{123, 62, 32, 32}, 16}

    edit_SquareBk:                  ThemeImageRect = {{1, 57, 27, 27}, 5}
    edit_SquareBorder:              ThemeImageRect = {{29, 57, 27, 27}, 5}
    edit_RoundBk:                   ThemeImageRect = {{1, 85, 27, 27}, 13}
    edit_RoundBorder:               ThemeImageRect = {{29, 85, 27, 27}, 13}
    edit_FlatBk:                    ThemeImageRect = {{1, 113, 27, 27}, 5}
    edit_FlatBorder:                ThemeImageRect = {{29, 113, 27, 27}, 5}

    btnToggle_Bk:                   ThemeImageRect = {{1, 141, 27, 27}, 13}
    btnToggle_Border:               ThemeImageRect = {{29, 141, 27, 27}, 13}
    btnToggle_Mark:                 ThemeImageRect = {{57, 141, 27, 27}, 13}

    slider_BkLeft:                  ThemeImageRect = {{1, 169, 27, 27}, 13, false}
    slider_BkMiddle:                ThemeImageRect = {{29, 169, 27, 27}, 13, false}
    slider_BkRight:                 ThemeImageRect = {{57, 169, 27, 27}, 13, false}
    slider_BkLeftVert:              ThemeImageRect = {{1, 309, 27, 27}, 13, false}
    slider_BkMiddleVert:            ThemeImageRect = {{29, 309, 27, 27}, 13, false}
    slider_BkRightVert:             ThemeImageRect = {{57, 309, 27, 27}, 13, false}
    slider_Mark:                    ThemeImageRect = {{86, 170, 25, 25}, 12}
    slider_Step:                    ThemeImageRect = {{113, 169, 27, 27}, 13}

    circular_Mark:                  ThemeImageRect = {{113, 337, 27, 27}, 13}

    tabItem_FlatSelected:           ThemeImageRect = {{57, 95, 27, 27}, 5}
    tabItem_RoundSelected:          ThemeImageRect = {{85, 95, 39, 25}, 10}
    tabItem_RoundHot:               ThemeImageRect = {{85, 95, 39, 25}, 10}
    tabItem_ButtonSelected:         ThemeImageRect = {{113, 309, 27, 27}, 9}
    tabItem_ButtonHot:              ThemeImageRect = {{113, 309, 27, 27}, 9}
    tabItem_PushButtonSelected:     ThemeImageRect = {{113, 309, 27, 27}, 13}
    tabItem_PushButtonHot:          ThemeImageRect = {{113, 309, 27, 27}, 13}
    tabItem_PushButtonBk:           ThemeImageRect = {{113, 309, 27, 27}, 13}
    tabItem_PushButtonBorder:       ThemeImageRect = {{141, 309, 27, 27}, 13}
    tabBar_FlatBk:                  ThemeImageRect = {{57, 95, 27, 27}, 5}
    tabBar_FlatRoundBk:             ThemeImageRect = {{57, 253, 27, 27}, 13}
    tabBar_RoundBk:                 ThemeImageRect = {{85, 309, 27, 27}, 9}
    tabView_FlatBk:                 ThemeImageRect = {{125, 95, 27, 27}, 10}
    tabView_RoundBk:                ThemeImageRect = {{153, 95, 27, 27}, 10}

    menuPopup_Shadow:               ThemeImageRect = {{189, 1, 66, 66}, 24}
    menuPopup_Bk:                   ThemeImageRect = {{1, 197, 27, 27}, 12}
    menuPopup_Border:               ThemeImageRect = {{29, 197, 27, 27}, 12}
    menuPopup_HotBk:                ThemeImageRect = {{57, 197, 27, 27}, 12}
    menuBar_Bk:                     ThemeImageRect = {{1, 225, 27, 27}, 12}
    menuBar_Border:                 ThemeImageRect = {{29, 225, 27, 27}, 5}
    menuBar_SelectedBk:             ThemeImageRect = {{57, 197, 27, 27}, 12}
    menuItem_CheckedMark:           ThemeImageRect = {{156, 62, 32, 32}, 0}

    scrollBar_BkV:                  ThemeImageRect = {{1, 253, 27, 27}, 0}
    scrollBar_BkH:                  ThemeImageRect = {{1, 253, 27, 27}, 0}
    scrollBar_BoxV:                 ThemeImageRect = {{29, 253, 27, 27}, 10}
    scrollBar_BoxH:                 ThemeImageRect = {{29, 253, 27, 27}, 10}
    scrollBar_Corner:               ThemeImageRect = {{1, 253, 27, 27}, 0}

    frameWnd_SmallShadow:           ThemeImageRect = {{189, 1, 66, 66}, 24}
    frameWnd_SquareBk:              ThemeImageRect = {{1, 57, 27, 27}, 5}
    frameWnd_SquareBorder:          ThemeImageRect = {{29, 57, 27, 27}, 5}
    frameWnd_RoundBk:               ThemeImageRect = {{1, 85, 27, 27}, 13}
    frameWnd_RoundBorder:           ThemeImageRect = {{29, 85, 27, 27}, 13}
    frameWnd_AnchorTop:             ThemeImageRect = {{141, 253, 27, 27}, 0}
    frameWnd_AnchorTopBk:           ThemeImageRect = {{141, 281, 27, 27}, 0}
    frameWnd_AnchorBottom:          ThemeImageRect = {{169, 253, 27, 27}, 0}
    frameWnd_AnchorBottomBk:        ThemeImageRect = {{169, 281, 27, 27}, 0}
    frameWnd_AnchorRight:           ThemeImageRect = {{197, 253, 27, 27}, 0}
    frameWnd_AnchorRightBk:         ThemeImageRect = {{197, 281, 27, 27}, 0}
    frameWnd_AnchorLeft:            ThemeImageRect = {{225, 253, 27, 27}, 0}
    frameWnd_AnchorLeftBk:          ThemeImageRect = {{225, 281, 27, 27}, 0}

    header_FlatBk:                  ThemeImageRect = {{57, 225, 27, 27}, 13}
    header_RoundBk:                 ThemeImageRect = {{57, 253, 27, 27}, 13}

    selectionBox_Bk:                ThemeImageRect = {{57, 197, 27, 27}, 12}

    splitView_HorzBarBk:            ThemeImageRect = {{85, 197, 27, 27}, 0}
    splitView_HorzBarHotBk:         ThemeImageRect = {{113, 197, 27, 27}, 0}
    splitView_VertBarBk:            ThemeImageRect = {{85, 225, 27, 27}, 0}
    splitView_VertBarHotBk:         ThemeImageRect = {{113, 225, 27, 27}, 0}

    progressBar_Bk:                 ThemeImageRect = {{113, 141, 27, 27}, 13}
    progressBar_Border:             ThemeImageRect = {{141, 141, 27, 27}, 13}
    progressBar_Mark:               ThemeImageRect = {{169, 141, 27, 27}, 13}

    colorPicker_RoundPick:          ThemeImageRect = {{141, 169, 27, 27}, 13}
    colorPicker_SquarePick:         ThemeImageRect = {{169, 169, 27, 27}, 13}
    colorPicker_Bk:                 ThemeImageRect = {{141, 197, 27, 27}, 13}
    colorPicker_Border:             ThemeImageRect = {{169, 197, 27, 27}, 13}
}
