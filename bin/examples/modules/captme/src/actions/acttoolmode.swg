using Gui

newCmdId("ModeShape")
newCmdId("ModeLine")
newCmdId("ModeArrow")
newCmdId("ModePolygon")
newCmdId("ModeText")
newCmdId("ModeFavorites")
newCmdId("ModeSelection")
newCmdId("ModeDraw")
newCmdId("ModeHighlight")
newCmdId("ModeStep")
newCmdId("ModeImage")
newCmdId("ModePixSel")

struct ActionToolMode {}
impl IActionUI for ActionToolMode
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdId.ModeShape, CmdId.ModeLine, CmdId.ModeArrow, CmdId.ModePolygon, CmdId.ModeText, CmdId.ModeFavorites, CmdId.ModeSelection, CmdId.ModeDraw, CmdId.ModeHighlight, CmdId.ModeStep, CmdId.ModeImage, CmdId.ModePixSel:
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
        case CmdId.ModeShape:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 0)
            evt.name    = "Shape"
            evt.toolTip = "Add circles and rectangle to the capture"
            evt.checked = main.editMode == .Shape

        case CmdId.ModeArrow:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 3)
            evt.name    = "Arrow"
            evt.toolTip = "Add arrows to the capture"
            evt.checked = main.editMode == .Arrow

        case CmdId.ModeLine:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 1)
            evt.name    = "Line"
            evt.toolTip = "Add lines to the capture"
            evt.checked = main.editMode == .Line

        case CmdId.ModePolygon:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 2)
            evt.name    = "Polygon"
            evt.toolTip = "Add polygons to the capture"
            evt.checked = main.editMode == .Polygon

        case CmdId.ModeText:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 4)
            evt.name    = "Text"
            evt.toolTip = "Add some text to the capture"
            evt.checked = main.editMode == .Text

        case CmdId.ModeDraw:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 7)
            evt.name    = "Draw"
            evt.toolTip = "Add some free hand paint to the capture"
            evt.checked = main.editMode == .Draw

        case CmdId.ModeHighlight:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 9)
            evt.name    = "Highlight"
            evt.toolTip = "Highlight some parts of the capture"
            evt.checked = main.editMode == .Highlight

        case CmdId.ModeStep:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 12)
            evt.name    = "Step"
            evt.toolTip = "Add some steps"
            evt.checked = main.editMode == .Step

        case CmdId.ModeImage:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 13)
            evt.name    = "Image"
            evt.toolTip = "Add some images/effects"
            evt.checked = main.editMode == .Image

        case CmdId.ModePixSel:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 17)
            evt.name    = "PixSel"
            evt.toolTip = "Pixel selection"
            evt.checked = main.editMode == .PixSel

        case CmdId.ModeFavorites:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 5)
            evt.name    = "Favorites"
            evt.toolTip = "Save multiple quick styles in one single location"
            evt.checked = main.editMode == .Favorites

        case CmdId.ModeSelection:
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
        case CmdId.ModeShape:
            main.editMode = .Shape
        case CmdId.ModeArrow:
            main.editMode = .Arrow
        case CmdId.ModeLine:
            main.editMode = .Line
        case CmdId.ModePolygon:
            main.editMode = .Polygon
        case CmdId.ModeText:
            main.editMode = .Text
        case CmdId.ModeDraw:
            main.editMode = .Draw
        case CmdId.ModeHighlight:
            main.editMode = .Highlight
        case CmdId.ModeStep:
            main.editMode = .Step
        case CmdId.ModeImage:
            main.editMode = .Image
        case CmdId.ModePixSel:
            main.editMode = .PixSel
        case CmdId.ModeFavorites:
            main.editMode = .Favorites
        case CmdId.ModeSelection:
            main.editMode = .Selection
        }

        main.updateState()
        return true
    }
}
