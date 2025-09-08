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

        if options.sigCreate:
            options.sigCreate(dlg)

        dlg.setIcon(options.icon)
        dlg.setIconColor(options.iconColor)
        dlg.sendResizeEvent()
        return dlg
    }

    // Display the box, centered, and returns the id of the pressed button
    mtd doModal(from: *Surface)->string
    {
        me.adaptSizeToMessage()
        me.surface.center(from)
        return Dialog.doModal(me)
    }

    internal mtd adaptSizeToMessage()
    {
        let metrics = me.getThemeMetrics()

        // Evaluate size of text
        var fake: Painter{fake: true}
        var rsf:  RichStringFormat
        rsf.font = me.getFont()
        rsf.flags |= .WordWrap
        rsf.horzAlignment = me.label.horzAlignmentText
        fake.drawRichString({0, 0, 512, 0}, &me.label.name, rsf)
        me.label.name.invalidate()

        // Width
        var w = 0'f32
        foreach btn in me.buttons:
            w += btn.position.width
        w += cast(f32) ((me.buttons.count - 1) * metrics.dialog_BtnPadding)
        w += 2 * metrics.dialog_Padding
        w += metrics.btnPush_Width

        var wl = me.label.name.boundRect.width + 2 * metrics.dialog_Padding
        wl += 2 * metrics.wnd_BorderSize
        w = Math.max(w, wl)

        if me.extension
        {
            var wl1 = me.extension.position.width + 2 * metrics.dialog_Padding
            wl1 += 2 * metrics.wnd_BorderSize
            w = Math.max(w, wl1)
        }

        // Height
        var h = Math.min(512, me.label.name.boundRect.height + me.label.padding.y + me.label.padding.w)
        if me.labelIcon:
            h += me.labelIcon.position.height
        else:
            h += metrics.dialog_Padding
        h += me.wndBottom.position.height
        h += 2 * metrics.wnd_BorderSize
        if me.surface.flags & .SmallCaption:
            h += metrics.wnd_SmallCaptionCY
        elif me.surface.flags & .BigCaption:
            h += metrics.wnd_BigCaptionCY

        if me.extension:
            h += me.extension.position.height

        var pos = me.surface.position
        pos.width  = w
        pos.height = h
        let sdw = me.surface.app.theme.metrics.surfaceWnd_ShadowSize
        pos.width += sdw * 2
        pos.height += sdw * 2
        me.surface.setPosition(pos)
    }

    // Add a bottom extended window
    mtd setExtension(wndExtension: *Wnd)
    {
        Debug.assert(me.extension == null)
        me.extension           = wndExtension
        me.extension.dockStyle = .Bottom
        me.extension.margin.x  = me.getThemeMetrics().dialog_Padding
    }

    // Set big icon color (icon must has been set before)
    mtd setIconColor(color: Color)
    {
        Debug.assert(me.labelIcon != null)
        me.labelIcon.iconColor = color
    }

    // Set big icon
    mtd setIcon(icon: Icon, color: Color = Argb.Zero)
    {
        if !icon.isValid():
            return

        if !me.labelIcon:
            me.labelIcon = Label.create(me.surface.wnd.view, "", {0, 0, 0, icon.sizeX + icon.sizeX * 0.5})
        me.label.padding.y = 0

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
            opt.icon = from.app.theme.getIcon64(ThemeIcons64.Question)
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
            opt.icon = from.app.theme.getIcon64(ThemeIcons64.Question)
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
        while !txt.isEmpty() and txt.back() == '\n':
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
        if !txt.isEmpty():
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
            opt.icon = from.app.theme.getIcon64(.Critical)
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
