using Core, Win32, Gdi32, Pixel

struct InPlaceEditWnd
{
    using wnd:  Wnd
    editWnd:    *EditWnd
    copyBtn:    *IconButton
    editBtn:    *IconButton
    exitBtn:    *IconButton
}

impl InPlaceEditWnd
{
    func create(capture: *Capture, main: *MainWnd, parent: *Wnd)->*InPlaceEditWnd
    {
        with let frame = Wnd.create'InPlaceEditWnd(parent)
        {
            .editWnd = EditWnd.create(main, frame)
            .editWnd.dockStyle = .Center
            .editWnd.editView.dockStyle = .Center
            .editWnd.editView.viewMargin = 0
            .editWnd.editView.setCapture(capture)
            .editWnd.editView.setZoom(1)
            .editWnd.editView.setFocus()
            .editWnd.editView.inPlaceEdit = true

            .editBtn = IconButton.create(.editWnd.editView, {}, {0, 0, 100, 20})
            .editBtn.name ="Edit"
            .editBtn.setForm(.Round)
            .editBtn.sigPressed += func|frame|(btn)
            {
                frame.cropCapture()
                g_Exit = true
                g_GrabDone = true
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
        let rect = getGrabRect()
        editBtn.setPosition(rect.right() - 100, rect.bottom(), 100, 40)
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
