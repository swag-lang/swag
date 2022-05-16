using Gui, Pixel, Core

enum EditMode
{
    Shapes
    Line
}

struct MainWnd
{
    using wnd: Wnd
    icons48:    ImageList
    editWnd:    *EditWnd
    editMode:   EditMode = .Shapes
}

const Icons48 = #load("ressources/icons48.png")

impl IWnd for MainWnd
{
    mtd onCommandUIEvent(evt: *CommandUIEvent)
    {
        editView := cast(*EditView) editWnd.view
        capture  := editView.capture

        hasSel       := capture and capture.selection.count
        hasSingleSel := capture and capture.selection.count == 1

        switch evt.id
        {
        case CmdIdAlignLeft:
            evt.setFlags |= .Disabled
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdAlignCenter:
            evt.setFlags |= .Disabled
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdAlignRight:
            evt.setFlags |= .Disabled
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdAlignTop:
            evt.setFlags |= .Disabled
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdAlignMiddle:
            evt.setFlags |= .Disabled
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdAlignBottom:
            evt.setFlags |= .Disabled
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdBringToFront:
            evt.setFlags |= .Disabled | .Icon
            evt.disabled = !hasSel or capture.selection.contains(capture.forms.count - 1)
            evt.icon = evt.target.getTheme().getIcon24(.BringToFront)
            evt.accepted = true

        case CmdIdSendToBack:
            evt.setFlags |= .Disabled | .Icon
            evt.disabled = !hasSel or capture.selection.contains(0)
            evt.icon = evt.target.getTheme().getIcon24(.SendToBack)
            evt.accepted = true

        case CmdIdBringForward:
            evt.setFlags |= .Disabled | .Icon | .Shortcut
            evt.disabled = !hasSel or capture.selection.contains(capture.forms.count - 1)
            evt.icon = evt.target.getTheme().getIcon24(.BringForward)
            evt.accepted = true

        case CmdIdSendBackward:
            evt.setFlags |= .Disabled | .Icon
            evt.disabled = !hasSel or capture.selection.contains(0)
            evt.icon = evt.target.getTheme().getIcon24(.SendBackward)
            evt.accepted = true

        case CmdIdUndo:
            evt.setFlags |= .Disabled
            evt.disabled = !capture or !capture.canUndo()
            evt.accepted = true

        case CmdIdRedo:
            evt.setFlags |= .Disabled
            evt.disabled = !capture or !capture.canRedo()
            evt.accepted = true
        }
    }

    mtd onCommandEvent(evt: *CommandEvent)
    {
        editView := cast(*EditView) editWnd.view
        capture  := editView.capture

        var undo: *Undo
        switch evt.id
        {
        case CmdIdAlignLeft:
            undo = capture.newUndo(.AlignLeft)
            evt.accepted = true
        case CmdIdAlignCenter:
            undo = capture.newUndo(.AlignCenter)
            evt.accepted = true
        case CmdIdAlignRight:
            undo = capture.newUndo(.AlignRight)
            evt.accepted = true
        case CmdIdAlignTop:
            undo = capture.newUndo(.AlignTop)
            evt.accepted = true
        case CmdIdAlignMiddle:
            undo = capture.newUndo(.AlignMiddle)
            evt.accepted = true
        case CmdIdAlignBottom:
            undo = capture.newUndo(.AlignBottom)
            evt.accepted = true
        case CmdIdBringToFront:
            undo = capture.newUndo(.MoveToFront)
            evt.accepted = true
        case CmdIdSendToBack:
            undo = capture.newUndo(.MoveToBack)
            evt.accepted = true
        case CmdIdBringForward:
            undo = capture.newUndo(.MoveForward)
            evt.accepted = true
        case CmdIdSendBackward:
            undo = capture.newUndo(.MoveBackward)
            evt.accepted = true
        case CmdIdUndo:
            capture.undo()
            evt.accepted = true
        case CmdIdRedo:
            capture.redo()
            evt.accepted = true
        }

        if undo
            capture.pushUndo(undo)
        if evt.accepted
            invalidate()
    }
}

impl MainWnd
{
    mtd createToolbar()
    {
        mtd createBtn(toolbar: *Wnd, n: string, iconIdx: s32)
        {
            b := IconButton.create(toolbar, Icon.from(&icons48, iconIdx), @{0, 0, 56, 64})
            b.name = n
            b.style.addStyleSheetColors("btnIcon_CheckedBk    $btnIcon_HotBk")
            b.style.addStyleSheetColors("btnIcon_CheckedHotBk $btnIcon_HotBk")
            b.iconBtnFlags |= .CheckableToggle
            b.checkedForm = .Bottom
            b.dockStyle = .Left
        }

        toolbar := Wnd.create'Wnd(self, @{0, 0, 0, 64})
        toolbar.dockStyle = .Top
        toolbar.backgroundStyle = .Window
        toolbar.style.addStyleSheetColors("wnd_Bk $wnd_Caption")
        self.createBtn(toolbar, "shape", 0)
        self.createBtn(toolbar, "line", 1)
    }

    func create(parent: *Wnd)->*MainWnd
    {
        res := Wnd.create'MainWnd(parent)
        renderer := parent.getApp().getRenderer()

        img := assume Image.decode(".png", Icons48)
        res.icons48.set(renderer.addImage(img), 48, 48)

        res.createToolbar()
        res.editWnd = EditWnd.create(res)
        res.editWnd.dockStyle = .Center

        res.registerKeyShortcut(Input.KeyModifiers.Control, Input.Key.Z, CmdIdUndo)
        res.registerKeyShortcut(Input.KeyModifiers.Control, Input.Key.Y, CmdIdRedo)
        res.registerKeyShortcut(Input.KeyModifiers.CtrlShift, Input.Key.F, CmdIdBringForward)
        res.registerKeyShortcut(Input.KeyModifiers.CtrlShift, Input.Key.B, CmdIdSendBackward)

        return res
    }
}