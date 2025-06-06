using Gui

const CmdIdRecentSmall  = "CmdIdRecentSmall"
const CmdIdRecentMedium = "CmdIdRecentMedium"
const CmdIdRecentBig    = "CmdIdRecentBig"
const CmdIdLibSmall     = "CmdIdLibSmall"
const CmdIdLibMedium    = "CmdIdLibMedium"
const CmdIdLibBig       = "CmdIdLibBig"

struct ActionRecentSize {}
impl IActionUI for ActionRecentSize
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdIdRecentSmall, CmdIdRecentMedium, CmdIdRecentBig:
            return true
        case CmdIdLibSmall, CmdIdLibMedium, CmdIdLibBig:
            return true            
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case CmdIdRecentSmall:
            evt.setFlags |= .Name | .ToolTip | .Checked | .Icon
            evt.toolTip = "Small recent view"
            evt.checked = main.bottomSize == .Small
            evt.icon    = Icon.from(&main.icons24, 0)

        case CmdIdRecentMedium:
            evt.setFlags |= .Name | .ToolTip | .Checked | .Icon
            evt.toolTip = "Medium recent view"
            evt.checked = main.bottomSize == .Medium
            evt.icon    = Icon.from(&main.icons24, 1)

        case CmdIdRecentBig:
            evt.setFlags |= .Name | .ToolTip | .Checked | .Icon
            evt.toolTip = "Big recent view"
            evt.checked = main.bottomSize == .Big
            evt.icon    = Icon.from(&main.icons24, 2)

        case CmdIdLibSmall:
            evt.setFlags |= .Name | .Checked
            evt.name = "Small view"
            evt.checked = main.libWnd.previewSize == .Small

        case CmdIdLibMedium:
            evt.setFlags |= .Name | .Checked
            evt.name = "Medium view"
            evt.checked = main.libWnd.previewSize == .Medium

        case CmdIdLibBig:
            evt.setFlags |= .Name | .Checked
            evt.name = "Big view"
            evt.checked = main.libWnd.previewSize == .Big
      
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case CmdIdRecentSmall:
            main.setBottomSize(.Small)

        case CmdIdRecentMedium:
            main.setBottomSize(.Medium)

        case CmdIdRecentBig:
            main.setBottomSize(.Big)

        case CmdIdLibSmall:
            main.libWnd.setPreviewSize(.Small)

        case CmdIdLibMedium:
            main.libWnd.setPreviewSize(.Medium)

        case CmdIdLibBig:
            main.libWnd.setPreviewSize(.Big)   
        }

        return true
    }
}

impl MainWnd
{
    mtd setBottomSize(bottomSize: BottomSize)
    {
        self.bottomSize = #self.bottomSize
        switch self.bottomSize
        {
        case Small:
            recentBar.resize(0, 80)
        case Medium:
            recentBar.resize(0, 128)
        case Big:
            recentBar.resize(0, 224)
        }

        applyLayout({position.width, position.height})
    }
}
