using Core, Pixel

public struct AboutDlg
{
    using dialog:          Dialog
    labelIcon:             *Label
    labelMessageTitle:     *Label
    labelMessage:          *Label
}

public struct AboutDlgParams
{
    dlgTitle:         string
    messageTitle:     string
    message:          string
    smallIcon:        Icon       // Icon that will be displayed in the title bar
    bigIcon:          Icon       // Big icon, inside the dialog
}

public impl AboutDlg
{
    // Creates the message box
    func create(from: *Surface, params: AboutDlgParams)->*AboutDlg
    {
        let dlg = Wnd.create'AboutDlg()
        let srf = dlg.createSurface(from, 0, 0, 500, 300)
        srf.setTitle(params.dlgTitle)

        let metrics = from.wnd.getThemeMetrics()

        with let stackLayout = StackLayoutCtrl.create(srf.getView())
        {
            .dockStyle = .Center
            .padding.y = params.bigIcon.sizeY * 0.25
        }

        with dlg.labelMessageTitle = Label.create(stackLayout, params.messageTitle, {})
        {
            let fontSize = cast(s32) (dlg.getThemeMetrics().defaultFontSize * 1.5)
            .style.setFont(dlg.getTheme().createDefaultFontfamily(fontSize))
            .dockStyle         = .Top
            .vertAlignmentText = .Top
            .horzAlignmentText = .Justify
        }

        with dlg.labelMessage = Label.create(stackLayout, params.message, {})
        {
            .dockStyle         = .Top
            .vertAlignmentText = .Top
            .horzAlignmentText = .Justify
        }

        if params.smallIcon.isValid()
        {
            var cpyIcon = params.smallIcon
            cpyIcon.sizeX, cpyIcon.sizeY = dlg.getThemeMetrics().wnd_BigCaptionCY * 0.8
            srf.setIcon(cpyIcon)
        }

        if params.bigIcon.isValid()
        {
            let widthIconPanel = params.bigIcon.sizeX + params.bigIcon.sizeX * 0.5
            with dlg.labelIcon = Label.create(srf.wnd.view, "", {0, 0, widthIconPanel, 0})
            {
                .dockStyle         = .Left
                .iconColor         = Argb.White
                .padding.y         = params.bigIcon.sizeY * 0.25
                .icon              = params.bigIcon
                .horzAlignmentIcon = .Center
                .vertAlignmentIcon = .Top
            }
        }

        dlg.addButton("Close", Dialog.BtnYes)
        dlg.sendResizeEvent()
        return dlg
    }

    // Display the box, centered, and returns the id of the pressed button
    mtd doModal(from: *Surface)
    {
        surface.center(from)
        discard Dialog.doModal(self)
    }
}
