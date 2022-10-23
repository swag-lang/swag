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
    label:  *Label
}

impl EmbInfoCtrl
{
    // Create the popup list, but do not display it
    func create(parent: *Wnd, name: string, kind: EmbInfoCtrlKind, height = 40, top = true)->*EmbInfoCtrl
    {
        theme  := parent.getTheme()
        colors := parent.getThemeColors()

        with ctrl := Wnd.create'EmbInfoCtrl(parent, @{0, 0, 0, height})
        {
            .dockStyle = top ? .Top : .Bottom
            .label = Label.create(ctrl, #self.name, @{})
            .label.backgroundStyle = .Window

            switch kind
            {
            case Critical:
                .label.icon = theme.getIcon64(.Critical, 28)
                .label.style.addStyleSheetColors(Format.toString("wnd_Bk 0x%{X}", colors.embInfo_CriticalBk.argb))
            case Information:
                .label.icon = theme.getIcon64(.Information, 28)
                .label.style.addStyleSheetColors(Format.toString("wnd_Bk 0x%{X}", colors.embInfo_InformationBk.argb))
            }

            .label.horzAlignmentText = .Left
            .label.horzAlignmentIcon = .Left
            .label.paddingIcon.x = 10
            .label.paddingText.x = 50
            .label.paddingText.y = 4
            .label.dockStyle = .Center

            ic := theme.getIcon24(.WndClose, 18)
            btn := IconButton.create(.label, ic, @{0, 0, 32, 32})
            btn.margin = 5
            btn.dockStyle = .Right
            btn.sigPressed += @|ctrl|(btn) {
                ctrl.destroy()
            }
        }

        return ctrl
    }
}
