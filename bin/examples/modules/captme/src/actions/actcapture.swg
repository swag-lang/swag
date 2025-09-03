using Gui

struct ActionCapture {}

impl ActionCapture
{
    newCmdId("Capture")
    newCmdId("Area")
    newCmdId("LastArea")
    newCmdId("FullScreen")
    newCmdId("Screen0")
    newCmdId("Screen1")
    newCmdId("Screen2")
    newCmdId("Screen3")
    newCmdId("ActiveWnd")
    newCmdId("HideEditor")
    newCmdId("ToInPlace")
    newCmdId("ToEditor")
    newCmdId("ToClipboard")
}

impl IActionUI for ActionCapture
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case ActionCapture.Capture, ActionCapture.Area, ActionCapture.LastArea, ActionCapture.FullScreen,
             ActionCapture.Screen0, ActionCapture.Screen1, ActionCapture.Screen2, ActionCapture.Screen3,
             ActionCapture.ActiveWnd, ActionCapture.HideEditor, ActionCapture.ToInPlace, ActionCapture.ToEditor,
             ActionCapture.ToClipboard:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case ActionCapture.Capture:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture"
            evt.icon = Icon.from(&main.icons48, 6)
        case ActionCapture.Area:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Area"
            evt.icon = Icon.from(&main.icons24, 7)
        case ActionCapture.LastArea:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Last Area"
            evt.icon = Icon.from(&main.icons24, 15)
        case ActionCapture.FullScreen:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Full Screen"
            evt.icon = Icon.from(&main.icons24, 6)
        case ActionCapture.ActiveWnd:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Active Window"
            evt.icon = Icon.from(&main.icons24, 14)
        case ActionCapture.Screen0:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 0 (%x%)", main.monitors[0].rect.width, main.monitors[0].rect.height)
            evt.icon = Icon.from(&main.icons24, 5)
        case ActionCapture.Screen1:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 1 (%x%)", main.monitors[1].rect.width, main.monitors[1].rect.height)
            evt.icon = Icon.from(&main.icons24, 5)
        case ActionCapture.Screen2:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 2 (%x%)", main.monitors[2].rect.width, main.monitors[2].rect.height)
            evt.icon = Icon.from(&main.icons24, 5)
        case ActionCapture.Screen3:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 3 (%x%)", main.monitors[3].rect.width, main.monitors[3].rect.height)
            evt.icon = Icon.from(&main.icons24, 5)

        case ActionCapture.HideEditor:
            evt.setFlags |= .Name | .Checked
            evt.name     = "Hide Editor Before Capture"
            evt.checked  = main.captureOptions.hideEditorBefore
            evt.accepted = true
        case ActionCapture.ToInPlace:
            evt.setFlags |= .Name | .Checked
            evt.name     = "Edit Captured Area In Place"
            evt.checked  = main.captureOptions.inPlaceEdit
            evt.accepted = true
        case ActionCapture.ToEditor:
            evt.setFlags |= .Name | .Checked
            evt.name     = "Send Capture To Editor"
            evt.checked  = main.captureOptions.sendToEditor
            evt.accepted = true
        case ActionCapture.ToClipboard:
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
        case ActionCapture.Capture:
            main.captureScreen(.Area)
        case ActionCapture.Area:
            main.captureScreen(.Area)
        case ActionCapture.LastArea:
            main.captureScreen(.LastArea)
        case ActionCapture.FullScreen:
            main.captureScreen(.FullScreen)
        case ActionCapture.ActiveWnd:
            main.captureScreen(.ActiveWindow)
        case ActionCapture.Screen0:
            main.captureScreen(.Screen0)
        case ActionCapture.Screen1:
            main.captureScreen(.Screen1)
        case ActionCapture.Screen2:
            main.captureScreen(.Screen2)
        case ActionCapture.Screen3:
            main.captureScreen(.Screen3)

        case ActionCapture.ToInPlace:
            main.captureOptions.inPlaceEdit = !main.captureOptions.inPlaceEdit
        case ActionCapture.HideEditor:
            main.captureOptions.hideEditorBefore = !main.captureOptions.hideEditorBefore
        case ActionCapture.ToEditor:
            main.captureOptions.sendToEditor = !main.captureOptions.sendToEditor
        case ActionCapture.ToClipboard:
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
