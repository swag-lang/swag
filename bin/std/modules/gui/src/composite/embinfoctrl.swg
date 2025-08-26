#global marked
#global public
using Core, Pixel

enum EmbInfoCtrlKind
{
    Critical
    Information
    Message
}

#[Swag.EnumFlags]
enum EmbInfoCtrlFlags
{
    Zero
    Top
    Bottom
    AutoHide
}

struct EmbInfoCtrl
{
    using frameWnd:     FrameWnd
    label:              *Label
    ctrlFlags:          EmbInfoCtrlFlags = .Zero
    sigOnClose:         SigArray'func||(*EmbInfoCtrl)
    timer:              *Timer
}

impl IWnd for EmbInfoCtrl
{
    mtd impl onTimerEvent(evt: *TimerEvent)
    {
        me.timer = null
        me.destroy()
    }

    mtd impl onDestroyEvent(evt: *DestroyEvent)
    {
        if me.timer:
            me.getApp().deleteTimer(me.timer)
    }
}

impl EmbInfoCtrl
{
    // Create the popup list, but do not display it
    func create(parent: *Wnd, name: string, kind: EmbInfoCtrlKind, flags = EmbInfoCtrlFlags.Top, height = 40)->*EmbInfoCtrl
    {
        let theme  = parent.getTheme()
        let colors = parent.getThemeColors()

        with let ctrl = Wnd.create'EmbInfoCtrl(parent, {0, 0, 0, height})
        {
            .wndFlags.add(.NoScroll)
            .dockStyle = flags.has(.Top) ? .Top : .Bottom
            .label     = Label.create(ctrl, #me.name, {})
            .ctrlFlags = flags
        }

        with ctrl.label
        {
            .backgroundStyle = .Window
            switch kind
            {
            case Critical:
                .icon = theme.getIcon64(.Critical, 28)
                .style.addStyleSheetColors(Format.toString("wnd_Bk 0x%{X}", colors.embInfo_CriticalBk.argb))
                .textColor = colors.embInfo_CriticalText.argb
            case Information:
                .icon = theme.getIcon64(.Information, 28)
                .style.addStyleSheetColors(Format.toString("wnd_Bk 0x%{X}", colors.embInfo_InformationBk.argb))
                .textColor = colors.embInfo_InformationText.argb
            case Message:
                .icon = theme.getIcon64(.Information, 28)
                .style.addStyleSheetColors(Format.toString("wnd_Bk 0x%{X}", colors.embInfo_MessageBk.argb))
                .textColor = colors.embInfo_MessageText.argb
            }

            .horzAlignmentText = .Left
            .horzAlignmentIcon = .Left
            .paddingIcon.x     = 10
            .paddingText.x     = 50
            .paddingText.y     = 4
            .dockStyle         = .Center
        }

        with let btn = IconButton.create(ctrl.label, theme.getIcon24(.WndClose, 18), {0, 0, 32, 32})
        {
            .margin       = 5
            .dockStyle    = .Right
            .forceFgColor = ctrl.label.textColor
            .sigPressed += func|ctrl|(btn)
            {
                ctrl.sigOnClose.call(ctrl)
                ctrl.destroy()
            }
        }

        if flags.has(.AutoHide):
            ctrl.timer = ctrl.addTimer(4's)

        parent.applyLayout()
        return ctrl
    }
}
