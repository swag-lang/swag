using Gui, Pixel, Core

enum EditMode
{
    Shape
    Arrow
    Line
    Polygon
}

struct MainWnd
{
    using wnd: Wnd
    icons48:    ImageList
    toolbar:    *Wnd
    editWnd:    *EditWnd

    editMode:       EditMode = .Shape
    mdlFormRect:    FormRect
    mdlFormArrow:   FormArrow
    mdlFormLine:    FormLine
    mdlFormPolygon: FormPolygon
}

const Icons48 = #load("ressources/icons48.png")

impl IWnd for MainWnd
{
    mtd onCommandUIEvent(evt: *CommandUIEvent)
    {
        editView := editWnd  ? cast(*EditView) editWnd.view : null
        capture  := editView ? editView.capture : null

        hasSel       := capture and capture.selection.count
        hasSingleSel := capture and capture.selection.count == 1

        switch evt.id
        {
        case CmdIdDeleteSelection:
            evt.setFlags |= .Disabled | .Name
            evt.disabled = !hasSel
            evt.name = "Delete"
            evt.accepted = true

        case CmdIdModeShape:
            evt.setFlags |= .Icon | .Checked | .Name
            evt.icon = Icon.from(&icons48, 0)
            evt.name = "shape"
            evt.checked = editMode == .Shape
            evt.accepted = true

        case CmdIdModeArrow:
            evt.setFlags |= .Icon | .Checked | .Name
            evt.icon = Icon.from(&icons48, 3)
            evt.name = "arrow"
            evt.checked = editMode == .Arrow
            evt.accepted = true

        case CmdIdModeLine:
            evt.setFlags |= .Icon | .Checked | .Name
            evt.icon = Icon.from(&icons48, 1)
            evt.name = "line"
            evt.checked = editMode == .Line
            evt.accepted = true

        case CmdIdModePolygon:
            evt.setFlags |= .Icon | .Checked | .Name
            evt.icon = Icon.from(&icons48, 2)
            evt.name = "polygon"
            evt.checked = editMode == .Polygon
            evt.accepted = true

        case CmdIdAlignLeft:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Left"
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdAlignCenter:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Center"
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdAlignRight:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Right"
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdAlignTop:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Top"
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdAlignMiddle:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Middle"
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdAlignBottom:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Bottom"
            evt.disabled = !hasSel or hasSingleSel
            evt.accepted = true

        case CmdIdBringToFront:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name = "Bring To Front"
            evt.disabled = !hasSel or capture.selection.contains(capture.forms.count - 1)
            evt.icon = evt.target.getTheme().getIcon24(.BringToFront)
            evt.accepted = true

        case CmdIdSendToBack:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name = "Send To Back"
            evt.disabled = !hasSel or capture.selection.contains(0)
            evt.icon = evt.target.getTheme().getIcon24(.SendToBack)
            evt.accepted = true

        case CmdIdBringForward:
            evt.setFlags |= .Disabled | .Icon | .Shortcut | .Name
            evt.name = "Bring Forward"
            evt.disabled = !hasSel or capture.selection.contains(capture.forms.count - 1)
            evt.icon = evt.target.getTheme().getIcon24(.BringForward)
            evt.accepted = true

        case CmdIdSendBackward:
            evt.setFlags |= .Disabled | .Icon | .Name
            evt.name = "Send Backward"
            evt.disabled = !hasSel or capture.selection.contains(0)
            evt.icon = evt.target.getTheme().getIcon24(.SendBackward)
            evt.accepted = true

        case CmdIdUndo:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Undo"
            evt.disabled = !capture or !capture.canUndo()
            evt.accepted = true

        case CmdIdRedo:
            evt.setFlags |= .Disabled | .Name
            evt.name = "Redo"
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
        case CmdIdDeleteSelection:
            undo = capture.newUndo(.DeleteSelection)
            evt.accepted = true

        case CmdIdModeShape:
            editMode = .Shape
            evt.accepted = true
            toolbar.updateCommandState()

        case CmdIdModeArrow:
            editMode = .Arrow
            evt.accepted = true
            toolbar.updateCommandState()

        case CmdIdModeLine:
            editMode = .Line
            evt.accepted = true
            toolbar.updateCommandState()

        case CmdIdModePolygon:
            editMode = .Polygon
            evt.accepted = true
            toolbar.updateCommandState()

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
        mtd createBtn(id: WndId)
        {
            b := IconButton.create(toolbar, position: @{0, 0, 56, 64})
            b.id = #self.id
            b.style.addStyleSheetColors("btnIcon_CheckedBk    $btnIcon_HotBk")
            b.style.addStyleSheetColors("btnIcon_CheckedHotBk $btnIcon_HotBk")
            b.iconBtnFlags |= .Checkable
            b.checkedForm = .Bottom
            b.dockStyle = .Left
        }

        toolbar = Wnd.create'Wnd(self, @{0, 0, 0, 64})
        toolbar.dockStyle = .Top
        toolbar.backgroundStyle = .Window
        toolbar.style.addStyleSheetColors("wnd_Bk $wnd_Caption")
        self.createBtn(CmdIdModeShape)
        self.createBtn(CmdIdModeArrow)
        self.createBtn(CmdIdModeLine)
        self.createBtn(CmdIdModePolygon)
        toolbar.updateCommandState()
    }

    func create(parent: *Wnd)->*MainWnd
    {
        res := Wnd.create'MainWnd(parent)
        renderer := parent.getApp().getRenderer()

        img := assume Image.decode(".png", Icons48)
        res.icons48.set(renderer.addImage(img), 48, 48)

        res.createToolbar()
        res.editWnd = EditWnd.create(res, res)
        res.editWnd.dockStyle = .Center

        res.registerKeyShortcut(Input.KeyModifiers.Control, Input.Key.Z, CmdIdUndo)
        res.registerKeyShortcut(Input.KeyModifiers.Control, Input.Key.Y, CmdIdRedo)
        res.registerKeyShortcut(Input.KeyModifiers.CtrlShift, Input.Key.F, CmdIdBringForward)
        res.registerKeyShortcut(Input.KeyModifiers.CtrlShift, Input.Key.B, CmdIdSendBackward)
        res.registerKeyShortcut(Input.KeyModifiers.None, Input.Key.Delete, CmdIdDeleteSelection)

        return res
    }
}