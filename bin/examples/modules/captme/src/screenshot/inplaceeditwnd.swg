using Core, Win32, Gdi32, Pixel

struct InPlaceEditWnd
{
    using wnd:     Wnd
    editWnd:       *EditWnd
    stackHorz:     *IconBar
    capture:       *Capture
}

impl InPlaceEditWnd
{
    func create(capture: *Capture, main: *MainWnd, parent: *Wnd)->*InPlaceEditWnd
    {
        // Generate a main window
        let theme = main.getTheme()
        let newMain = Wnd.create'MainWnd(main.parent, {})
        newMain.inPlaceEdit = true

        with let frame = Wnd.create'InPlaceEditWnd(parent)
        {
            .capture           = capture
            .editWnd           = EditWnd.create(newMain, frame)
            .editWnd.dockStyle = .Center
            newMain.editWnd    = .editWnd

            with let view = frame.editWnd.editView
            {
                .dockStyle  = .Center
                .viewMargin = 0
                .setCapture(capture)
                .setZoom(1)
                .setFocus()
                .inPlaceEdit = true
            }

            .stackHorz         = IconBar.create(.editWnd.editView, 24, {})
            .stackHorz.addItem(theme.getIcon24(ThemeIcons24.Copy), "copy")
            .stackHorz.addItem(theme.getIcon24(ThemeIcons24.Edit), "edit")
            .stackHorz.addItem(theme.getIcon24(ThemeIcons24.WndClose), "close")
            .stackHorz.sigPressed += func|frame|(btn)
            {
                switch btn.id
                {
                case "copy":
                    InPlaceEditWnd.cropCapture(frame.capture)
                    frame.capture.copyAll()
                    g_Exit     = true
                    g_GrabDone = false
                case "close":
                    g_Exit     = true
                    g_GrabDone = false
                case "edit":
                    InPlaceEditWnd.cropCapture(frame.capture)
                    g_Exit     = true
                    g_GrabDone = true
                }
            }

            .stackHorz.padding = 5
        }

        capture.gizmo.sigHasChanged = func|frame|()
        {
            frame.moveButtons()
        }

        frame.moveButtons()
        return frame
    }

    func cropCapture(capture: *Capture)
    {
        var grabRect: Math.Rectangle
        with capture
        {
            grabRect.x      = -.backImagePos.x
            grabRect.y      = -.backImagePos.y
            grabRect.width  = .width
            grabRect.height = .height

            let scale = Env.getDPIScale()
            grabRect.x *= scale
            grabRect.y *= scale
            grabRect.width *= scale
            grabRect.height *= scale

            .backImg.crop(grabRect)
            .backImgOrg = .backImg
            .width, .widthOrg = cast(s32) .backImg.width
            .height, .heightOrg = cast(s32) .backImg.height
            .backImagePos = {}

            .gizmo.sigHasChanged = null
            .inPlaceEdit         = false
            .dirtyBackImg        = true

            .selection.clear()
        }
    }

    mtd moveButtons()
    {
        var rect: Math.Rectangle
        rect.x      = -capture.backImagePos.x
        rect.y      = -capture.backImagePos.y
        rect.width  = capture.width
        rect.height = capture.height

        const H = 40.0
        const W = 100.0

        let x = Math.max(10, rect.right() - W)
        if rect.bottom() < g_ScreenHeight - H:
            stackHorz.setPosition(x, rect.bottom(), W, H)
        elif rect.y > H:
            stackHorz.setPosition(x, rect.y - H, W, H)
        else:
            stackHorz.setPosition(x, rect.bottom() - H, W, H)
    }
}

impl IWnd for InPlaceEditWnd
{
    mtd impl onKeyEvent(evt: *KeyEvent)
    {
        if evt.kind == .KeyPressed and evt.key == .Escape:
            g_Exit = true
    }
}
