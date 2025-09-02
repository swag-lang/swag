using Gui

newCmdId("RecentSmall")
newCmdId("RecentMedium")
newCmdId("RecentBig")
newCmdId("LibSmall")
newCmdId("LibMedium")
newCmdId("LibBig")

struct ActionRecentSize {}
impl IActionUI for ActionRecentSize
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case CmdId.RecentSmall, CmdId.RecentMedium, CmdId.RecentBig:
            return true
        case CmdId.LibSmall, CmdId.LibMedium, CmdId.LibBig:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case CmdId.RecentSmall:
            evt.setFlags |= .Name | .ToolTip | .Checked | .Icon
            evt.toolTip = "Small recent view"
            evt.checked = main.bottomSize == .Small
            evt.icon    = Icon.from(&main.icons24, 0)

        case CmdId.RecentMedium:
            evt.setFlags |= .Name | .ToolTip | .Checked | .Icon
            evt.toolTip = "Medium recent view"
            evt.checked = main.bottomSize == .Medium
            evt.icon    = Icon.from(&main.icons24, 1)

        case CmdId.RecentBig:
            evt.setFlags |= .Name | .ToolTip | .Checked | .Icon
            evt.toolTip = "Big recent view"
            evt.checked = main.bottomSize == .Big
            evt.icon    = Icon.from(&main.icons24, 2)

        case CmdId.LibSmall:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "Small view"
            evt.checked       = main.libWnd.previewSize == .Small

        case CmdId.LibMedium:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "Medium view"
            evt.checked       = main.libWnd.previewSize == .Medium

        case CmdId.LibBig:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "Big view"
            evt.checked       = main.libWnd.previewSize == .Big
        }

        return true
    }

    mtd impl execute(cxt: ActionContext)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case CmdId.RecentSmall:
            main.setBottomSize(.Small)

        case CmdId.RecentMedium:
            main.setBottomSize(.Medium)

        case CmdId.RecentBig:
            main.setBottomSize(.Big)

        case CmdId.LibSmall:
            main.libWnd.setPreviewSize(.Small)

        case CmdId.LibMedium:
            main.libWnd.setPreviewSize(.Medium)

        case CmdId.LibBig:
            main.libWnd.setPreviewSize(.Big)
        }

        return true
    }
}

impl MainWnd
{
    mtd setBottomSize(bottomSize: BottomSize)
    {
        me.bottomSize = bottomSize
        switch me.bottomSize
        {
        case Small:
            me.recentBar.resize(0, 80)
        case Medium:
            me.recentBar.resize(0, 128)
        case Big:
            me.recentBar.resize(0, 224)
        }

        me.applyLayout({me.position.width, me.position.height})
    }
}
