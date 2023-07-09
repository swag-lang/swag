#global public
using Core, Pixel

enum EmbInfoCtrlKind
{
    Critical
    Information
}

struct EmbInfoCtrl
{
    using frameWnd: FrameWnd
    label: *Label
}

impl EmbInfoCtrl
{
    // Create the popup list, but do not display it
    func create(parent: *Wnd, name: string, kind: EmbInfoCtrlKind, height = 40, top = true)->*EmbInfoCtrl
    {
        var theme = parent.getTheme()
        var colors = parent.getThemeColors()

        with var ctrl = Wnd.create'EmbInfoCtrl(parent, {0, 0, 0, height})
        {
            .dockStyle = top ? .Top : .Bottom
            .label = Label.create(ctrl, #self.name, {})
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
            .paddingIcon.x = 10
            .paddingText.x = 50
            .paddingText.y = 4
            .dockStyle = .Center
        }

        with var btn = IconButton.create(ctrl.label, theme.getIcon24(.WndClose, 18), {0, 0, 32, 32})
        {
            .margin = 5
            .dockStyle = .Right
            .sigPressed += closure|ctrl|(btn) {
                ctrl.destroy()
            }
        }

        return ctrl
    }
}
