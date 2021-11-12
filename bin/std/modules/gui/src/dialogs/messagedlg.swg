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
        dlg.IWnd.onMouseEvent(evt)
    }
}

public impl MessageDlg
{
    const BtnYes = "MessageDlg.yes"
    const BtnNo  = "MessageDlg.no"

    func create(from: *Surface, x, y: s32, message: string)->*MessageDlg
    {
        dlg := Wnd.create'MessageDlg(null)

        surface := assume from.app.createSurface(x, y, 500, 300, SurfaceFlags.Border | SurfaceFlags.Secondary, dlg)
        surface.wnd.view.backgroundStyle = .Dialog

        dlg.label = Label.create(surface.wnd.view, message, @{})
        dlg.label.dockStyle = .Center
        dlg.label.padding = @{10, 10, 10, 10}

        dlg.wndBottom = Wnd.create'Wnd(surface.wnd.view, "", @{0, 0, 0, 50})
        dlg.wndBottom.dockStyle = .Bottom
        dlg.wndBottom.backgroundStyle = .DialogBar
        dlg.wndBottom.padding = @{10, 10, 10, 10}

        return dlg
    }

    // Set big icon
    mtd setIcon(icon: Icon)
    {
        if !labelIcon
        {
            labelIcon = Label.create(surface.wnd.view, "", @{0,0,0,100})
            labelIcon.dockStyle = .Top
            labelIcon.padding = @{10, 10, 10, 10}
        }

        labelIcon.icon = icon
    }

    // Add a button
    mtd addButton(str, id: string, isDefault: bool)->*ButtonPush
    {
        btn := ButtonPush.create(wndBottom, str, @{0, 0, 100, 30})
        btn.id = #scopefct.id
        btn.dockStyle = .Right
        btn.isDefault = isDefault
        btn.margin = @{5, 0, 5, 0}
        return btn
    }

    // Message box with a 'yes' and 'no' button
    func yesNo(from: *Surface, message: string, defaultId = BtnNo)->string
    {
        dlg := create(from, 0, 0, message)

        dlg.setIcon(from.app.theme.getIcon64(ThemeIcons64.Question))

        btn := dlg.addButton("No", BtnNo, defaultId == BtnNo)
        btn.sigPressed = @(btn: *ButtonPush) { btn.surface.app.exitModal(BtnNo); }

        btn = dlg.addButton("Yes", BtnYes, defaultId == BtnYes)
        btn.sigPressed = @(btn: *ButtonPush) { btn.surface.app.exitModal(BtnYes); }

        dlg.surface.center(from)
        return Dialog.doModal(dlg.surface)
    }
}