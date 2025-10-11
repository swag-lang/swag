using Core, Pixel

public struct MessageDlg
{
    using dialog:     Dialog
    labelIcon:        *Label
    label:            *Label
    extension:        *Wnd
}

public struct MessageDlgOptions
{
    icon:                 Icon
    iconColor:            Color
    horzAlignmentText     = StringHorzAlignment.Center
    vertAlignmentText     = StringVertAlignment.Center
    sigCreate:            func||(*MessageDlg)
}

public impl MessageDlg
{
    // Creates the message box
    func create(from: *Surface, message: string, options: MessageDlgOptions)->*MessageDlg
    {
        let dlg = Wnd.create'MessageDlg()
        dlg.fromSurface = from

        let srf = dlg.createSurface(from, 0, 0, 500, 300)

        let metrics = from.wnd.getThemeMetrics()

        with dlg.label = Label.create(srf.getView(), message, {})
        {
            .dockStyle = .Center
            .labelFlags.add(.WordWrap)
            .horzAlignmentText = options.horzAlignmentText
            .vertAlignmentText = options.vertAlignmentText
            .setAllPaddings(metrics.dialog_Padding)
        }

        if options.sigCreate do
            options.sigCreate(dlg)

        dlg.setIcon(options.icon)
        dlg.setIconColor(options.iconColor)
        dlg.sendResizeEvent()
        return dlg
    }

    // Display the box, centered, and returns the id of the pressed button
    mtd doModal(from: *Surface)->string
    {
        .adaptSizeToMessage()
        .surface.center(from)
        return Dialog.doModal(me)
    }

    internal mtd adaptSizeToMessage()
    {
        let metrics = .getThemeMetrics()

        // Evaluate size of text
        var fake: Painter{fake: true}
        var rsf:  RichStringFormat
        rsf.font = .getFont()
        rsf.flags |= .WordWrap
        rsf.horzAlignment = .label.horzAlignmentText
        fake.drawRichString({0, 0, 512, 0}, &.label.name, rsf)
        .label.name.invalidate()

        // Width
        var w = 0'f32
        foreach btn in .buttons do
            w += btn.position.width
        w += cast(f32) ((.buttons.count - 1) * metrics.dialog_BtnPadding)
        w += 2 * metrics.dialog_Padding
        w += metrics.btnPush_Width

        var wl = .label.name.boundRect.width + 2 * metrics.dialog_Padding
        wl += 2 * metrics.wnd_BorderSize
        w = Math.max(w, wl)

        if .extension
        {
            var wl1 = .extension.position.width + 2 * metrics.dialog_Padding
            wl1 += 2 * metrics.wnd_BorderSize
            w = Math.max(w, wl1)
        }

        // Height
        var h = Math.min(512, .label.name.boundRect.height + .label.padding.y + .label.padding.w)
        if .labelIcon do
            h += .labelIcon.position.height
        else do
            h += metrics.dialog_Padding
        h += .wndBottom.position.height
        h += 2 * metrics.wnd_BorderSize
        if .surface.flags & .SmallCaption do
            h += metrics.wnd_SmallCaptionCY
        elif .surface.flags & .BigCaption do
            h += metrics.wnd_BigCaptionCY

        if .extension do
            h += .extension.position.height

        var pos = .surface.position
        pos.width  = w
        pos.height = h
        let sdw = .surface.app.theme.metrics.surfaceWnd_ShadowSize
        pos.width += sdw * 2
        pos.height += sdw * 2
        .surface.setPosition(pos)
    }

    // Add a bottom extended window
    mtd setExtension(wndExtension: *Wnd)
    {
        Debug.assert(.extension == null)
        .extension           = wndExtension
        .extension.dockStyle = .Bottom
        .extension.margin.x  = .getThemeMetrics().dialog_Padding
    }

    // Set big icon color (icon must has been set before)
    mtd setIconColor(color: Color)
    {
        Debug.assert(.labelIcon != null)
        .labelIcon.iconColor = color
    }

    // Set big icon
    mtd setIcon(icon: Icon, color: Color = Argb.Zero)
    {
        if !icon.isValid() do
            return

        if !.labelIcon do
            .labelIcon = Label.create(.surface.wnd.view, "", {0, 0, 0, icon.sizeX + icon.sizeX * 0.5})
        .label.padding.y = 0

        with me.labelIcon
        {
            .dockStyle         = .Top
            .iconColor         = color
            .icon              = icon
            .horzAlignmentIcon = .Center
            .vertAlignmentIcon = .Center
        }
    }

    // Message box with a 'ok'
    func ok(from: *Surface, message: string, options: MessageDlgOptions = {})
    {
        with let dlg = create(from, message, options)
        {
            .addButton("OK", Dialog.BtnOk, true)
        }

        discard dlg.doModal(from)
    }

    // Message box with a 'ok' and 'cancel' button
    func okCancel(from: *Surface, message: string, defaultId = Dialog.BtnCancel, options: MessageDlgOptions = {})->WndId
    {
        with let dlg = create(from, message, options)
        {
            .addButton("OK", Dialog.BtnOk, defaultId == Dialog.BtnOk)
            .addButton("Cancel", Dialog.BtnCancel, defaultId == Dialog.BtnCancel)
        }

        return dlg.doModal(from)
    }

    // Message box with a 'yes' and 'no' button
    func yesNo(from: *Surface, message: string, title: string = null, defaultId = Dialog.BtnNo, options: MessageDlgOptions = {})->WndId
    {
        var opt = options
        if !opt.icon.isValid()
        {
            opt.icon      = from.app.theme.getIcon64(ThemeIcons64.Question)
            opt.iconColor = Argb.White
        }

        with let dlg = create(from, message, opt)
        {
            .surface.setTitle(title)
            .addButton("Yes", Dialog.BtnYes, defaultId == Dialog.BtnYes)
            .addButton("No", Dialog.BtnNo, defaultId == Dialog.BtnNo)
        }

        return dlg.doModal(from)
    }

    // Message box with a 'yes' and 'no' button
    func yesNoCancel(from: *Surface, message: string, defaultId = Dialog.BtnCancel, options: MessageDlgOptions = {})->WndId
    {
        var opt = options
        if !opt.icon.isValid()
        {
            opt.icon      = from.app.theme.getIcon64(ThemeIcons64.Question)
            opt.iconColor = Argb.White
        }

        with let dlg = create(from, message, opt)
        {
            .addButton("Yes", Dialog.BtnYes, defaultId == Dialog.BtnYes)
            .addButton("No", Dialog.BtnNo, defaultId == Dialog.BtnNo)
            .addButton("Cancel", Dialog.BtnCancel, defaultId == Dialog.BtnCancel)
        }

        return dlg.doModal(from)
    }

    // Message box to 'confirm' something
    func confirm(from: *Surface, message: string, question: string = null, defaultId = Dialog.BtnNo, options: MessageDlgOptions = {})->WndId
    {
        var txt = String.from(message)
        while !txt.isEmpty() and txt.back() == '\n' do
            txt.removeBack()
        txt += "\n\n"
        txt += question orelse "<b>Do you want to continue ?</b>"

        return yesNo(from, txt.toString(), "Please confirm", defaultId, options)
    }

    // Message box to show the current raised error.
    // @err must still contain the raised error.
    func error(from: *Surface, message: string = null, options: MessageDlgOptions = {})
    {
        var txt: String = String.from(message)
        if !txt.isEmpty() do
            txt += "\n"

        if @err != null
        {
            switch @err
            {
            case Swag.SystemError:
                let err = cast(Swag.SystemError) @err
                txt += err.message
            }
        }

        var opt = options
        if !opt.icon.isValid()
        {
            opt.icon      = from.app.theme.getIcon64(.Critical)
            opt.iconColor = Argb.White
        }

        with let dlg = create(from, txt.toString(), opt)
        {
            .surface.setTitle("Error")
            .addButton("OK", Dialog.BtnOk, true)
        }

        discard dlg.doModal(from)
    }
}
