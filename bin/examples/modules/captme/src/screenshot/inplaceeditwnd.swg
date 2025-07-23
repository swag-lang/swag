using Core, Win32, Gdi32, Pixel

struct InPlaceEditWnd
{
    using wnd:  Wnd
    editWnd:    *EditWnd
    stackHorz:  *StackLayoutCtrl
}

impl InPlaceEditWnd
{
    func create(capture: *Capture, main: *MainWnd, parent: *Wnd)->*InPlaceEditWnd
    {
        with let frame = Wnd.create'InPlaceEditWnd(parent)
        {
            .editWnd = EditWnd.create(main, frame)
            .editWnd.dockStyle = .Center
            with let view = frame.editWnd.editView
            {
                .dockStyle = .Center
                .viewMargin = 0
                .setCapture(capture)
                .setZoom(1)
                .setFocus()
                .inPlaceEdit = true
            }

            .stackHorz = StackLayoutCtrl.create(.editWnd.editView, .Right, {})
            .stackHorz.backgroundStyle = .Dialog
            .stackHorz.padding         = 5

            let btnCancel = PushButton.create(.stackHorz, "Cancel", {0, 0, 100})
            btnCancel.sigPressed += func(btn)
            {
                g_Exit = true
                g_GrabDone = false
            }

            let btnEdit = PushButton.create(.stackHorz, "Edit", {0, 0, 100})
            btnEdit.sigPressed += func|frame|(btn)
            {
                frame.cropCapture()
                g_Exit = true
                g_GrabDone = true
            }

            let btnCopy = PushButton.create(.stackHorz, "Copy", {0, 0, 100})
            btnCopy.sigPressed += func|frame|(btn)
            {
                frame.cropCapture()
                Clipboard.addImage(frame.editWnd.editView.capture.backImg)
                g_Exit = true
                g_GrabDone = false
            }
        }

        frame.moveButtons()

        return frame
    }

    mtd cropCapture()
    {
        var grabRect: Math.Rectangle
        with editWnd.editView.capture
        {
            grabRect.x = -.backImagePos.x
            grabRect.y = -.backImagePos.y
            grabRect.width = .width
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

            .inPlaceEdit = false
            .dirtyBackImg = true
        }
    }

    mtd moveButtons()
    {
        var rect = getGrabRect()
        rect.y += 4
        stackHorz.setPosition(rect.x, rect.bottom(), rect.width, 40)
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
