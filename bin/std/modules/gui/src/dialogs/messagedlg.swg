using Core

public struct MessageDlg
{
    using dlg:  Dialog
    labelIcon:  *Label
    label:      *Label
    wndBottom:  *Wnd
}

impl IWnd for MessageDlg
{
    mtd onMouseEvent(evt: *MouseEvent)
    {
        dlg.base.IWnd.onMouseEvent(evt)
    }
}

impl MessageDlg
{
    public func create(from: *Surface, x, y: s32)->*MessageDlg
    {
        dlg := Wnd.create'MessageDlg(null, "", @{})

        surface := assume from.app.createSurface(x, y, 500, 300, SurfaceFlags.Border | SurfaceFlags.Secondary, dlg)
        surface.wnd.view.backgroundStyle = .Dialog

        dlg.label = Label.create(surface.wnd.view, "", @{})
        dlg.label.name = "Lorem ipsum dolor sit amet, consectetur <b>adipiscing<b> elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."
        dlg.label.dockStyle = .Center
        dlg.label.padding = @{10, 10, 10, 10}

        dlg.wndBottom = Wnd.create'Wnd(surface.wnd.view, "", @{0, 0, 0, 50})
        dlg.wndBottom.dockStyle = .Bottom
        dlg.wndBottom.backgroundStyle = .DialogBar
        dlg.wndBottom.padding = @{10, 10, 10, 10}

        return dlg
    }

    // Set big icon
    public mtd setIcon(ico: Icon)
    {
        if !labelIcon
        {
            labelIcon = Label.create(surface.wnd.view, "", @{0,0,0,100})
            labelIcon.dockStyle = .Top
            labelIcon.padding = @{10, 10, 10, 10}
        }

        labelIcon.icon = ico
    }

    // Add a button
    public mtd addButton(str: string, isDefault: bool)->*ButtonPush
    {
        btn := ButtonPush.create(wndBottom, str, @{0, 0, 100, 30})
        btn.dockStyle = .Right
        btn.isDefault = isDefault
        btn.margin = @{5, 0, 5, 0}
        return btn
    }

    // Message box with a 'yes' and 'no' button
    public func yesNo(from: *Surface, x, y: s32, message: string, defaultIsNo = false)
    {
        dlg := create(from, x, y)

        dlg.setIcon(from.app.theme.getIcon64(ThemeIcons64.Question))

        btn := dlg.addButton("No", defaultIsNo)
        btn.sigPressed = @(btn: *ButtonPush) { btn.surface.app.exitModal(0); }

        btn = dlg.addButton("Yes", !defaultIsNo)
        btn.sigPressed = @(btn: *ButtonPush) { btn.surface.app.exitModal(0); }

        dlg.surface.center(from)
        discard Dialog.doModal(dlg.surface)
    }
}