using Core, Win32, Gdi32, Pixel

struct InPlaceEditWnd
{
    using wnd:     Wnd
    editWnd:       *EditWnd
    stackHorz:     *StackLayoutCtrl
    capture:       *Capture
}

impl InPlaceEditWnd
{
    func create(capture: *Capture, main: *MainWnd, parent: *Wnd)->*InPlaceEditWnd
    {
        // Generate a main window
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

            .stackHorz         = StackLayoutCtrl.create(.editWnd.editView, .Right, {})
            .stackHorz.padding = 5
        }

        capture.gizmo.sigHasChanged = func|frame|()
        {
            frame.moveButtons()
        }

        let btnCancel = PushButton.create(frame.stackHorz, "Cancel", {0, 0, 70})
        btnCancel.sigPressed += func(btn)
        {
            g_Exit     = true
            g_GrabDone = false
        }

        let btnEdit = PushButton.create(frame.stackHorz, "Edit", {0, 0, 70})
        btnEdit.sigPressed += func|frame|(btn)
        {
            InPlaceEditWnd.cropCapture(frame.capture)
            g_Exit     = true
            g_GrabDone = true
        }

        let btnCopy = PushButton.create(frame.stackHorz, "Copy", {0, 0, 70})
        btnCopy.sigPressed += func|frame|(btn)
        {
            InPlaceEditWnd.cropCapture(frame.capture)
            frame.capture.copyAll()
            g_Exit     = true
            g_GrabDone = false
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
        const W = 250.0

        var x = rect.x
        var w = rect.width

        if rect.width < W
        {
            x = rect.right() - W
            w = rect.right() - x
        }

        x = Math.max(x, 20)
        w = Math.max(w, W)

        if rect.bottom() < g_ScreenHeight - H:
            stackHorz.setPosition(x, rect.bottom(), w, H)
        elif rect.y > H:
            stackHorz.setPosition(x, rect.y - H, w, H)
        else:
            stackHorz.setPosition(x, rect.bottom() - H, w, H)
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
