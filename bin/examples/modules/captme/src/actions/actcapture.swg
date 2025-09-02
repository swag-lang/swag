using Gui

newCmdId("Capture")
newCmdId("CaptureArea")
newCmdId("CaptureLastArea")
newCmdId("CaptureFullScreen")
newCmdId("CaptureScreen0")
newCmdId("CaptureScreen1")
newCmdId("CaptureScreen2")
newCmdId("CaptureScreen3")
newCmdId("CaptureActiveWnd")
newCmdId("CaptureHideEditor")
newCmdId("CaptureToInPlace")
newCmdId("CaptureToEditor")
newCmdId("CaptureToClipboard")

struct ActionCapture {}
impl IActionUI for ActionCapture
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdId.Capture, CmdId.CaptureArea, CmdId.CaptureLastArea, CmdId.CaptureFullScreen,
             CmdId.CaptureScreen0, CmdId.CaptureScreen1, CmdId.CaptureScreen2, CmdId.CaptureScreen3,
             CmdId.CaptureActiveWnd, CmdId.CaptureHideEditor, CmdId.CaptureToInPlace, CmdId.CaptureToEditor,
             CmdId.CaptureToClipboard:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case CmdId.Capture:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture"
            evt.icon = Icon.from(&main.icons48, 6)
        case CmdId.CaptureArea:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Area"
            evt.icon = Icon.from(&main.icons24, 7)
        case CmdId.CaptureLastArea:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Last Area"
            evt.icon = Icon.from(&main.icons24, 15)
        case CmdId.CaptureFullScreen:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Full Screen"
            evt.icon = Icon.from(&main.icons24, 6)
        case CmdId.CaptureActiveWnd:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Active Window"
            evt.icon = Icon.from(&main.icons24, 14)
        case CmdId.CaptureScreen0:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 0 (%x%)", main.monitors[0].rect.width, main.monitors[0].rect.height)
            evt.icon = Icon.from(&main.icons24, 5)
        case CmdId.CaptureScreen1:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 1 (%x%)", main.monitors[1].rect.width, main.monitors[1].rect.height)
            evt.icon = Icon.from(&main.icons24, 5)
        case CmdId.CaptureScreen2:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 2 (%x%)", main.monitors[2].rect.width, main.monitors[2].rect.height)
            evt.icon = Icon.from(&main.icons24, 5)
        case CmdId.CaptureScreen3:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 3 (%x%)", main.monitors[3].rect.width, main.monitors[3].rect.height)
            evt.icon = Icon.from(&main.icons24, 5)

        case CmdId.CaptureHideEditor:
            evt.setFlags |= .Name | .Checked
            evt.name     = "Hide Editor Before Capture"
            evt.checked  = main.captureOptions.hideEditorBefore
            evt.accepted = true
        case CmdId.CaptureToInPlace:
            evt.setFlags |= .Name | .Checked
            evt.name     = "Edit Captured Area In Place"
            evt.checked  = main.captureOptions.inPlaceEdit
            evt.accepted = true
        case CmdId.CaptureToEditor:
            evt.setFlags |= .Name | .Checked
            evt.name     = "Send Capture To Editor"
            evt.checked  = main.captureOptions.sendToEditor
            evt.accepted = true
        case CmdId.CaptureToClipboard:
            evt.setFlags |= .Name | .Checked
            evt.name     = "Send Capture To Clipboard"
            evt.checked  = main.captureOptions.sendToClipboard
            evt.accepted = true
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case CmdId.Capture:
            main.captureScreen(.Area)
        case CmdId.CaptureArea:
            main.captureScreen(.Area)
        case CmdId.CaptureLastArea:
            main.captureScreen(.LastArea)
        case CmdId.CaptureFullScreen:
            main.captureScreen(.FullScreen)
        case CmdId.CaptureActiveWnd:
            main.captureScreen(.ActiveWindow)
        case CmdId.CaptureScreen0:
            main.captureScreen(.Screen0)
        case CmdId.CaptureScreen1:
            main.captureScreen(.Screen1)
        case CmdId.CaptureScreen2:
            main.captureScreen(.Screen2)
        case CmdId.CaptureScreen3:
            main.captureScreen(.Screen3)

        case CmdId.CaptureToInPlace:
            main.captureOptions.inPlaceEdit = !main.captureOptions.inPlaceEdit
        case CmdId.CaptureHideEditor:
            main.captureOptions.hideEditorBefore = !main.captureOptions.hideEditorBefore
        case CmdId.CaptureToEditor:
            main.captureOptions.sendToEditor = !main.captureOptions.sendToEditor
        case CmdId.CaptureToClipboard:
            main.captureOptions.sendToClipboard = !main.captureOptions.sendToClipboard
        }

        return true
    }
}

impl MainWnd
{
    mtd newCapture(img: Image, linked: string = null, from: *Capture = null)
    {
        if !img.isValid():
            return

        var capture = from
        if !capture
        {
            with capture = Memory.new'Capture()
            {
                .width, .widthOrg = img.width
                .height, .heightOrg = img.height
                .backImg = img
            }
        }

        with capture
        {
            .changed     = true
            .linkedImage = linked
            .file        = Library.newFileName()
        }

        me.editWnd.editView.setCapture(capture)
        me.editWnd.editView.zoomToFit(false)

        capture.computePreview()
        me.recentWnd.addRecent(capture.file, capture.preview, linked: linked)
        me.recentWnd.recentView.selected = 0

        catch capture.save()
        g_Library.addFile(capture.file)

        // Force edition mode
        if me.libraryMode
        {
            me.swapLibraryMode()
            me.updateState()
        }
    }

    mtd captureScreen(mode: CaptureMode)
    {
        catch screenShot(me, mode)
    }
}
