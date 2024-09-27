using Gui

const CmdIdModeShape     = "CmdIdModeShape"
const CmdIdModeLine      = "CmdIdModeLine"
const CmdIdModeArrow     = "CmdIdModeArrow"
const CmdIdModePolygon   = "CmdIdModePolygon"
const CmdIdModeText      = "CmdIdModeText"
const CmdIdModeFavorites = "CmdIdModeFavorites"
const CmdIdModeSelection = "CmdIdModeSelection"
const CmdIdModeDraw      = "CmdIdModeDraw"
const CmdIdModeHighlight = "CmdIdModeHighlight"
const CmdIdModeStep      = "CmdIdModeStep"
const CmdIdModeImage     = "CmdIdModeImage"
const CmdIdModePixSel    = "CmdIdModePixSel"

struct ActionToolMode {}
impl IActionUI for ActionToolMode
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdModeShape, CmdIdModeLine, CmdIdModeArrow, CmdIdModePolygon, CmdIdModeText, CmdIdModeFavorites, CmdIdModeSelection, CmdIdModeDraw, CmdIdModeHighlight, CmdIdModeStep, CmdIdModeImage, CmdIdModePixSel:
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
        case CmdIdModeShape:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 0)
            evt.name    = "Shape"
            evt.toolTip = "Add circles and rectangle to the capture"
            evt.checked = main.editMode == .Shape

        case CmdIdModeArrow:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 3)
            evt.name    = "Arrow"
            evt.toolTip = "Add arrows to the capture"
            evt.checked = main.editMode == .Arrow

        case CmdIdModeLine:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 1)
            evt.name    = "Line"
            evt.toolTip = "Add lines to the capture"
            evt.checked = main.editMode == .Line

        case CmdIdModePolygon:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 2)
            evt.name    = "Polygon"
            evt.toolTip = "Add polygons to the capture"
            evt.checked = main.editMode == .Polygon

        case CmdIdModeText:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 4)
            evt.name    = "Text"
            evt.toolTip = "Add some text to the capture"
            evt.checked = main.editMode == .Text

        case CmdIdModeDraw:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 7)
            evt.name    = "Draw"
            evt.toolTip = "Add some free hand paint to the capture"
            evt.checked = main.editMode == .Draw

        case CmdIdModeHighlight:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 9)
            evt.name    = "Highlight"
            evt.toolTip = "Highlight some parts of the capture"
            evt.checked = main.editMode == .Highlight

        case CmdIdModeStep:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 12)
            evt.name    = "Step"
            evt.toolTip = "Add some steps"
            evt.checked = main.editMode == .Step

        case CmdIdModeImage:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 13)
            evt.name    = "Image"
            evt.toolTip = "Add some images/effects"
            evt.checked = main.editMode == .Image

        case CmdIdModePixSel:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 17)
            evt.name    = "PixSel"
            evt.toolTip = "Pixel selection"
            evt.checked = main.editMode == .PixSel

        case CmdIdModeFavorites:
            evt.setFlags |= .Icon | .Checked | .Name | .ToolTip
            evt.icon    = Icon.from(&main.icons48, 5)
            evt.name    = "Favorites"
            evt.toolTip = "Save multiple quick styles in one single location"
            evt.checked = main.editMode == .Favorites

        case CmdIdModeSelection:
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
        let main    = cast(*MainWnd) cxt.wnd
        let capture = main.getCapture()

        switch cxt.id
        {
        case CmdIdModeShape:
            main.editMode = .Shape
        case CmdIdModeArrow:
            main.editMode = .Arrow
        case CmdIdModeLine:
            main.editMode = .Line
        case CmdIdModePolygon:
            main.editMode = .Polygon
        case CmdIdModeText:
            main.editMode = .Text
        case CmdIdModeDraw:
            main.editMode = .Draw
        case CmdIdModeHighlight:
            main.editMode = .Highlight
        case CmdIdModeStep:
            main.editMode = .Step
        case CmdIdModeImage:
            main.editMode = .Image
        case CmdIdModePixSel:
            main.editMode = .PixSel
        case CmdIdModeFavorites:
            main.editMode = .Favorites
        case CmdIdModeSelection:
            main.lastEditMode = main.editMode
            main.editMode     = .Selection
        }

        capture.selection.clear()
        main.updateState()
        return true
    }
}
