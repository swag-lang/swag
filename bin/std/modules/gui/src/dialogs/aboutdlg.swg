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
    iconTitle:        Icon       // Icon that will be displayed in the title bar
    bigIcon:          Icon       // Big icon, inside the dialog
}

public impl AboutDlg
{
    // Creates the message box
    func create(from: *Surface, params: AboutDlgParams, width, height: s32 = 0)->*AboutDlg
    {
        let dlg = Wnd.create'AboutDlg()
        let w   = cast(s32) width orelse 500
        let h   = cast(s32) height orelse 300
        let srf = dlg.createSurface(from, 0, 0, w, h, .CloseBtn)
        srf.setTitle(params.dlgTitle)

        with let stackLayout = StackLayoutCtrl.create(srf.getView())
        {
            .dockStyle = .Center
            .padding.y = params.bigIcon.sizeY * 0.25
        }

        with dlg.labelMessageTitle = Label.create(stackLayout, params.messageTitle)
        {
            let fontSize = cast(s32) (dlg.getThemeMetrics().defaultFontSize * 1.5)
            .style.setFont(dlg.getTheme().createDefaultFontfamily(fontSize))
            .dockStyle         = .Top
            .vertAlignmentText = .Top
            .horzAlignmentText = .Left
            .labelFlags.add(.AutoHeight)
            .autoHeight()
        }

        with dlg.labelMessage = Label.create(stackLayout, params.message, flags: .AutoHeight | .WordWrap)
        {
            .dockStyle         = .Top
            .vertAlignmentText = .Top
            .horzAlignmentText = .Left
        }

        if params.iconTitle.isValid()
        {
            var cpyIcon = params.iconTitle
            cpyIcon.sizeX, cpyIcon.sizeY = dlg.getThemeMetrics().wnd_BigCaptionCY * 0.7
            srf.setIcon(cpyIcon)
        }

        if params.bigIcon.isValid()
        {
            let widthIconPanel = params.bigIcon.sizeX + params.bigIcon.sizeX * 0.5
            with dlg.labelIcon = Label.create(srf.wnd.view, "", {0, 0, widthIconPanel, 0})
            {
                .dockStyle         = .Left
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
        .surface.center(from)
        discard Dialog.doModal(me)
    }
}
