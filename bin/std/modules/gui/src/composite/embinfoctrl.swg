#global public
using Core, Pixel

enum EmbInfoCtrlKind
{
    Critical
    Information
}

struct EmbInfoCtrl
{
    using frameWnd:     FrameWnd
    label:              *Label
}

impl EmbInfoCtrl
{
    // Create the popup list, but: not display it
    func create(parent: nn *Wnd, name: string, kind: EmbInfoCtrlKind, height = 40, top = true)->*EmbInfoCtrl
    {
        let theme  = parent.getTheme()
        let colors = parent.getThemeColors()

        with let ctrl = Wnd.create'EmbInfoCtrl(parent, {0, 0, 0, height})
        {
            .dockStyle = top ? .Top : .Bottom
            .label     = Label.create(ctrl, #self.name, {})
        }

        with ctrl.label
        {
            .backgroundStyle = .Window
            switch kind
            {
            case Critical:
                .icon = theme.getIcon64(.Critical, 28)
                .style.addStyleSheetColors(Format.toString("wnd_Bk 0x%{X}", colors.embInfo_CriticalBk.argb))
            case Information:
                .icon = theme.getIcon64(.Information, 28)
                .style.addStyleSheetColors(Format.toString("wnd_Bk 0x%{X}", colors.embInfo_InformationBk.argb))
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
            .margin    = 5
            .dockStyle = .Right
            .sigPressed += func|ctrl|(btn)
            {
                ctrl.destroy()
            }
        }

        return ctrl
    }
}
