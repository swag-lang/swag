using Gui

struct ActionCapture {}

impl ActionCapture
{
    newCmdId("DoIt")
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
        case DoIt, Area, LastArea, FullScreen,
             Screen0, Screen1, Screen2, Screen3,
             ActiveWnd, HideEditor, ToInPlace, ToEditor,
             ToClipboard:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case DoIt:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture"
            evt.icon = Icon.from(&main.icons48, 6)
        case Area:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Area"
            evt.icon = Icon.from(&main.icons24, 7)
        case LastArea:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Last Area"
            evt.icon = Icon.from(&main.icons24, 15)
        case FullScreen:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Full Screen"
            evt.icon = Icon.from(&main.icons24, 6)
        case ActiveWnd:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Active Window"
            evt.icon = Icon.from(&main.icons24, 14)
        case Screen0:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 0 (%x%)", main.monitors[0].rect.width, main.monitors[0].rect.height)
            evt.icon = Icon.from(&main.icons24, 5)
        case Screen1:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 1 (%x%)", main.monitors[1].rect.width, main.monitors[1].rect.height)
            evt.icon = Icon.from(&main.icons24, 5)
        case Screen2:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 2 (%x%)", main.monitors[2].rect.width, main.monitors[2].rect.height)
            evt.icon = Icon.from(&main.icons24, 5)
        case Screen3:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 3 (%x%)", main.monitors[3].rect.width, main.monitors[3].rect.height)
            evt.icon = Icon.from(&main.icons24, 5)

        case HideEditor:
            evt.setFlags |= .Name | .Checked
            evt.name     = "Hide Editor Before Capture"
            evt.checked  = main.captureOptions.hideEditorBefore
            evt.accepted = true
        case ToInPlace:
            evt.setFlags |= .Name | .Checked
            evt.name     = "Edit Captured Area In Place"
            evt.checked  = main.captureOptions.inPlaceEdit
            evt.accepted = true
        case ToEditor:
            evt.setFlags |= .Name | .Checked
            evt.name     = "Send Capture To Editor"
            evt.checked  = main.captureOptions.sendToEditor
            evt.accepted = true
        case ToClipboard:
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
        case DoIt:
            main.captureScreen(.Area)
        case Area:
            main.captureScreen(.Area)
        case LastArea:
            main.captureScreen(.LastArea)
        case FullScreen:
            main.captureScreen(.FullScreen)
        case ActiveWnd:
            main.captureScreen(.ActiveWindow)
        case Screen0:
            main.captureScreen(.Screen0)
        case Screen1:
            main.captureScreen(.Screen1)
        case Screen2:
            main.captureScreen(.Screen2)
        case Screen3:
            main.captureScreen(.Screen3)

        case ToInPlace:
            main.captureOptions.inPlaceEdit = !main.captureOptions.inPlaceEdit
        case HideEditor:
            main.captureOptions.hideEditorBefore = !main.captureOptions.hideEditorBefore
        case ToEditor:
            main.captureOptions.sendToEditor = !main.captureOptions.sendToEditor
        case ToClipboard:
            main.captureOptions.sendToClipboard = !main.captureOptions.sendToClipboard
        }

        return true
    }
}

impl MainWnd
{
    mtd newCapture(img: Image, linked: string = null, from: *Capture = null)
    {
        if !img.isValid() do
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

        .editWnd.editView.setCapture(capture)
        .editWnd.editView.zoomToFit(false)

        capture.computePreview()
        .recentWnd.addRecent(capture.file, capture.preview, linked: linked)
        .recentWnd.recentView.selected = 0

        catch capture.save()
        g_Library.addFile(capture.file)

        // Force edition mode
        if .libraryMode
        {
            .swapLibraryMode()
            .updateState()
        }
    }

    mtd captureScreen(mode: CaptureMode)
    {
        catch screenShot(me, mode)
    }
}
