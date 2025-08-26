#global marked
using Gui

const CmdIdCapture            = "CmdIdCapture"
const CmdIdCaptureArea        = "CmdIdCaptureArea"
const CmdIdCaptureLastArea    = "CmdIdCaptureLastArea"
const CmdIdCaptureFullScreen  = "CmdIdCaptureFullScreen"
const CmdIdCaptureScreen0     = "CmdIdCaptureScreen0"
const CmdIdCaptureScreen1     = "CmdIdCaptureScreen1"
const CmdIdCaptureScreen2     = "CmdIdCaptureScreen2"
const CmdIdCaptureScreen3     = "CmdIdCaptureScreen3"
const CmdIdCaptureActiveWnd   = "CmdIdCaptureActiveWnd"
const CmdIdCaptureHideEditor  = "CmdIdCaptureHideEditor"
const CmdIdCaptureToInPlace   = "CmdIdCaptureToInPlace"
const CmdIdCaptureToEditor    = "CmdIdCaptureToEditor"
const CmdIdCaptureToClipboard = "CmdIdCaptureToClipboard"

struct ActionCapture {}
impl IActionUI for ActionCapture
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdCapture, CmdIdCaptureArea, CmdIdCaptureLastArea, CmdIdCaptureFullScreen,
             CmdIdCaptureScreen0, CmdIdCaptureScreen1, CmdIdCaptureScreen2, CmdIdCaptureScreen3,
             CmdIdCaptureActiveWnd, CmdIdCaptureHideEditor, CmdIdCaptureToInPlace, CmdIdCaptureToEditor,
             CmdIdCaptureToClipboard:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case CmdIdCapture:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture"
            evt.icon = Icon.from(&main.icons48, 6)
        case CmdIdCaptureArea:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Area"
            evt.icon = Icon.from(&main.icons24, 7)
        case CmdIdCaptureLastArea:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Last Area"
            evt.icon = Icon.from(&main.icons24, 15)
        case CmdIdCaptureFullScreen:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Full Screen"
            evt.icon = Icon.from(&main.icons24, 6)
        case CmdIdCaptureActiveWnd:
            evt.setFlags |= .Name | .Icon
            evt.name = "Capture Active Window"
            evt.icon = Icon.from(&main.icons24, 14)
        case CmdIdCaptureScreen0:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 0 (%x%)", main.monitors[0].rect.width, main.monitors[0].rect.height)
            evt.icon = Icon.from(&main.icons24, 5)
        case CmdIdCaptureScreen1:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 1 (%x%)", main.monitors[1].rect.width, main.monitors[1].rect.height)
            evt.icon = Icon.from(&main.icons24, 5)
        case CmdIdCaptureScreen2:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 2 (%x%)", main.monitors[2].rect.width, main.monitors[2].rect.height)
            evt.icon = Icon.from(&main.icons24, 5)
        case CmdIdCaptureScreen3:
            evt.setFlags |= .Name | .Icon
            evt.name = Format.toString("Capture Monitor 3 (%x%)", main.monitors[3].rect.width, main.monitors[3].rect.height)
            evt.icon = Icon.from(&main.icons24, 5)

        case CmdIdCaptureHideEditor:
            evt.setFlags |= .Name | .Checked
            evt.name     = "Hide Editor Before Capture"
            evt.checked  = main.captureOptions.hideEditorBefore
            evt.accepted = true
        case CmdIdCaptureToInPlace:
            evt.setFlags |= .Name | .Checked
            evt.name     = "Edit Captured Area In Place"
            evt.checked  = main.captureOptions.inPlaceEdit
            evt.accepted = true
        case CmdIdCaptureToEditor:
            evt.setFlags |= .Name | .Checked
            evt.name     = "Send Capture To Editor"
            evt.checked  = main.captureOptions.sendToEditor
            evt.accepted = true
        case CmdIdCaptureToClipboard:
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
        case CmdIdCapture:
            main.captureScreen(.Area)
        case CmdIdCaptureArea:
            main.captureScreen(.Area)
        case CmdIdCaptureLastArea:
            main.captureScreen(.LastArea)
        case CmdIdCaptureFullScreen:
            main.captureScreen(.FullScreen)
        case CmdIdCaptureActiveWnd:
            main.captureScreen(.ActiveWindow)
        case CmdIdCaptureScreen0:
            main.captureScreen(.Screen0)
        case CmdIdCaptureScreen1:
            main.captureScreen(.Screen1)
        case CmdIdCaptureScreen2:
            main.captureScreen(.Screen2)
        case CmdIdCaptureScreen3:
            main.captureScreen(.Screen3)

        case CmdIdCaptureToInPlace:
            main.captureOptions.inPlaceEdit = !main.captureOptions.inPlaceEdit
        case CmdIdCaptureHideEditor:
            main.captureOptions.hideEditorBefore = !main.captureOptions.hideEditorBefore
        case CmdIdCaptureToEditor:
            main.captureOptions.sendToEditor = !main.captureOptions.sendToEditor
        case CmdIdCaptureToClipboard:
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
