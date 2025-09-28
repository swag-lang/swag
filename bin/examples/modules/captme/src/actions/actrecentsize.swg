using Gui

struct ActionRecentSize {}

impl ActionRecentSize
{
    newCmdId("RecentSmall")
    newCmdId("RecentMedium")
    newCmdId("RecentBig")
    newCmdId("LibSmall")
    newCmdId("LibMedium")
    newCmdId("LibBig")
}

impl IActionUI for ActionRecentSize
{
    mtd impl accept(cxt: ActionContext)->bool
    {
        switch cxt.id
        {
        case RecentSmall, RecentMedium, RecentBig:
            return true
        case LibSmall, LibMedium, LibBig:
            return true
        }

        return false
    }

    mtd impl update(cxt: ActionContext, evt: *CommandStateEvent)->bool
    {
        let main = cast(*MainWnd) cxt.wnd

        switch cxt.id
        {
        case RecentSmall:
            evt.setFlags |= .Name | .ToolTip | .Checked | .Icon
            evt.toolTip = "Small recent view"
            evt.checked = main.bottomSize == .Small
            evt.icon    = Icon.from(&main.icons24, 0)

        case RecentMedium:
            evt.setFlags |= .Name | .ToolTip | .Checked | .Icon
            evt.toolTip = "Medium recent view"
            evt.checked = main.bottomSize == .Medium
            evt.icon    = Icon.from(&main.icons24, 1)

        case RecentBig:
            evt.setFlags |= .Name | .ToolTip | .Checked | .Icon
            evt.toolTip = "Big recent view"
            evt.checked = main.bottomSize == .Big
            evt.icon    = Icon.from(&main.icons24, 2)

        case LibSmall:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "Small view"
            evt.checked       = main.libWnd.previewSize == .Small

        case LibMedium:
            evt.setFlags |= .Name | .Checked | .RenderingHint
            evt.renderingHint = .ExclusiveGroup
            evt.name          = "Medium view"
            evt.checked       = main.libWnd.previewSize == .Medium

        case LibBig:
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
        case RecentSmall:
            main.setBottomSize(.Small)

        case RecentMedium:
            main.setBottomSize(.Medium)

        case RecentBig:
            main.setBottomSize(.Big)

        case LibSmall:
            main.libWnd.setPreviewSize(.Small)

        case LibMedium:
            main.libWnd.setPreviewSize(.Medium)

        case LibBig:
            main.libWnd.setPreviewSize(.Big)
        }

        return true
    }
}

impl MainWnd
{
    mtd setBottomSize(bottomSize: BottomSize)
    {
        .bottomSize = bottomSize
        switch .bottomSize
        {
        case Small:
            .recentBar.resize(0, 80)
        case Medium:
            .recentBar.resize(0, 128)
        case Big:
            .recentBar.resize(0, 224)
        }

        .applyLayout({.position.width, .position.height})
    }
}
