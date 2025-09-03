using Gui

struct ActionToolMode {}

impl ActionToolMode
{
    newCmdId("Shape")
    newCmdId("Line")
    newCmdId("Arrow")
    newCmdId("Polygon")
    newCmdId("Text")
    newCmdId("Favorites")
    newCmdId("Selection")
    newCmdId("Draw")
    newCmdId("Highlight")
    newCmdId("Step")
    newCmdId("Image")
    newCmdId("PixSel")
}

impl IActionUI for ActionToolMode
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case ActionToolMode.Shape, ActionToolMode.Line, ActionToolMode.Arrow, ActionToolMode.Polygon, ActionToolMode.Text, ActionToolMode.Favorites, ActionToolMode.Selection, ActionToolMode.Draw, ActionToolMode.Highlight, ActionToolMode.Step, ActionToolMode.Image, ActionToolMode.PixSel:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main    = cast(*MainWnd) cxt.wnd
        let capture = main.getCapture()

        switch cxt.id
        {
        case ActionToolMode.Shape:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 0)
            evt.name    = "Shape"
            evt.toolTip = "Add circles and rectangle to the capture"
            evt.checked = main.editMode == .Shape

        case ActionToolMode.Arrow:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 3)
            evt.name    = "Arrow"
            evt.toolTip = "Add arrows to the capture"
            evt.checked = main.editMode == .Arrow

        case ActionToolMode.Line:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 1)
            evt.name    = "Line"
            evt.toolTip = "Add lines to the capture"
            evt.checked = main.editMode == .Line

        case ActionToolMode.Polygon:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 2)
            evt.name    = "Polygon"
            evt.toolTip = "Add polygons to the capture"
            evt.checked = main.editMode == .Polygon

        case ActionToolMode.Text:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 4)
            evt.name    = "Text"
            evt.toolTip = "Add some text to the capture"
            evt.checked = main.editMode == .Text

        case ActionToolMode.Draw:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 7)
            evt.name    = "Draw"
            evt.toolTip = "Add some free hand paint to the capture"
            evt.checked = main.editMode == .Draw

        case ActionToolMode.Highlight:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 9)
            evt.name    = "Highlight"
            evt.toolTip = "Highlight some parts of the capture"
            evt.checked = main.editMode == .Highlight

        case ActionToolMode.Step:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 12)
            evt.name    = "Step"
            evt.toolTip = "Add some steps"
            evt.checked = main.editMode == .Step

        case ActionToolMode.Image:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 13)
            evt.name    = "Image"
            evt.toolTip = "Add some images/effects"
            evt.checked = main.editMode == .Image

        case ActionToolMode.PixSel:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 17)
            evt.name    = "PixSel"
            evt.toolTip = "Pixel selection"
            evt.checked = main.editMode == .PixSel

        case ActionToolMode.Favorites:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 5)
            evt.name    = "Favorites"
            evt.toolTip = "Save multiple quick styles in one single location"
            evt.checked = main.editMode == .Favorites

        case ActionToolMode.Selection:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 10)
            evt.name    = "Select"
            evt.toolTip = "Select forms"
            evt.checked = main.editMode == .Selection or (capture and capture.drawSelRect)
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main = cast(*MainWnd) cxt.wnd
        switch cxt.id
        {
        case ActionToolMode.Shape:
            main.editMode = .Shape
        case ActionToolMode.Arrow:
            main.editMode = .Arrow
        case ActionToolMode.Line:
            main.editMode = .Line
        case ActionToolMode.Polygon:
            main.editMode = .Polygon
        case ActionToolMode.Text:
            main.editMode = .Text
        case ActionToolMode.Draw:
            main.editMode = .Draw
        case ActionToolMode.Highlight:
            main.editMode = .Highlight
        case ActionToolMode.Step:
            main.editMode = .Step
        case ActionToolMode.Image:
            main.editMode = .Image
        case ActionToolMode.PixSel:
            main.editMode = .PixSel
        case ActionToolMode.Favorites:
            main.editMode = .Favorites
        case ActionToolMode.Selection:
            main.editMode = .Selection
        }

        main.updateState()
        return true
    }
}
