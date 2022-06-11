using Core, Gui, Pixel

struct QuickButton
{
    using iconBtn: IconButton
    formItf: IForm
}

struct QuickWnd
{
    using scrollWnd: ScrollWnd

    layout:         *WrapLayoutCtrl
    main:           *MainWnd
    qsArrow:        *QuickStyle
    qsShape:        *QuickStyle
    qsLine:         *QuickStyle
    qsPolygon:      *QuickStyle
    qsText:         *QuickStyle
    currentStyle:   *QuickStyle
}

impl IWnd for QuickWnd
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
    }
}

impl QuickWnd
{
    func create(parent: *Wnd)->*QuickWnd
    {
        quick := Wnd.create'QuickWnd(parent, @{0, 0, 0, 300})
        quick.backgroundStyle = .Window

        quick.layout = WrapLayoutCtrl.create(quick)
        quick.layout.padding = 6
        quick.layout.spacing = 10
        quick.setView(quick.layout)
        quick.loadQuickStyles()
        return quick
    }

    mtd loadQuickStyleArrow()
    {
        qsArrow = Memory.new'QuickStyle()

        #[Swag.Macro]
        func add(startSize, borderSize, endSize: f32, color: Color)
        {
            form := Memory.new'FormArrow()
            form.borderSize = borderSize
            form.startSize = startSize
            form.endSize = endSize
            if startSize != 0
                form.beginCap = .ArrowAnchor
            if endSize != 0
                form.endCap = .ArrowAnchor
            form.borderColor = color
            it := cast(IForm) form
            it.setup(null)
            `qsArrow.forms.add(it)
        }

        const BorderSize = 10.0
        const ArrowSize  = 3.0

        add(0, BorderSize, ArrowSize, Argb.Red)
        add(0, BorderSize, ArrowSize, Argb.Green)
        add(0, BorderSize, ArrowSize, Argb.Blue)
        add(0, BorderSize, ArrowSize, Argb.Yellow)

        add(ArrowSize, BorderSize, ArrowSize, Argb.Red)
        add(ArrowSize, BorderSize, ArrowSize, Argb.Green)
        add(ArrowSize, BorderSize, ArrowSize, Argb.Blue)
        add(ArrowSize, BorderSize, ArrowSize, Argb.Yellow)
    }

    mtd loadQuickStyleShape()
    {
        qsShape = Memory.new'QuickStyle()

        #[Swag.Macro]
        func add(borderSize: f32, kind: FormShapeKind, color: Color, backColor: Color)
        {
            form := Memory.new'FormShape()
            form.borderSize = borderSize
            form.borderColor = color
            form.backColor = backColor
            form.kind = kind
            it := cast(IForm) form
            it.setup(null)
            `qsShape.forms.add(it)
        }

        const BorderSize = 10.0
        add(BorderSize, .Ellipse, Argb.Red, Argb.Zero)
        add(BorderSize, .Ellipse, Argb.Purple, Argb.Zero)
        add(BorderSize, .Ellipse, Argb.Yellow, Argb.Zero)
        add(BorderSize, .RoundRectangle, Argb.Red, Argb.Zero)
        add(BorderSize, .RoundRectangle, Argb.Purple, Argb.Zero)
        add(BorderSize, .Rectangle, Argb.Red, Argb.Zero)
        add(BorderSize, .Rectangle, Argb.Purple, Argb.Zero)
        add(0, .Rectangle, Argb.Zero, Argb.White)
        add(0, .Rectangle, Argb.Zero, Argb.Black)
        add(BorderSize, .Rectangle, Argb.White, Argb.Zero)
        add(BorderSize, .Rectangle, Argb.Black, Argb.Zero)
    }

    mtd loadQuickStyleLine()
    {
        qsLine = Memory.new'QuickStyle()

        #[Swag.Macro]
        func add(borderSize: f32, color: Color)
        {
            form := Memory.new'FormLine()
            form.borderSize = borderSize
            form.borderColor = color
            it := cast(IForm) form
            it.setup(null)
            `qsLine.forms.add(it)
        }

        const BorderSize = 10.0
        add(BorderSize, Argb.White)
        add(BorderSize, Argb.Gray)
        add(BorderSize, Argb.Black)
        add(BorderSize, Argb.Red)
        add(BorderSize, Argb.Green)
        add(BorderSize, Argb.Blue)
        add(BorderSize, Argb.Yellow)
        add(BorderSize, Argb.Purple)
    }

    mtd loadQuickStylePolygon()
    {
        qsPolygon = Memory.new'QuickStyle()

        #[Swag.Macro]
        func add(borderSize: f32, color: Color)
        {
            form := Memory.new'FormPolygon()
            form.borderSize = borderSize
            form.borderColor = color
            form.backColor = 0
            it := cast(IForm) form
            it.setup(null)
            `qsPolygon.forms.add(it)
        }

        const BorderSize = 5.0
        add(BorderSize, Argb.Red)
        add(BorderSize, Argb.Green)
        add(BorderSize, Argb.Blue)
        add(BorderSize, Argb.Yellow)
        add(BorderSize, Argb.Purple)
        add(BorderSize, Argb.White)
        add(BorderSize, Argb.Black)
    }

    mtd loadQuickStyleText()
    {
        qsText = Memory.new'QuickStyle()

        #[Swag.Macro]
        func add(borderSize, fontSize: f32, bdColor, bkColor: Color)
        {
            form := Memory.new'FormText()
            form.borderSize = borderSize
            form.borderColor = bdColor
            form.backColor = bkColor
            form.fontSize = fontSize
            form.text = "A"
            it := cast(IForm) form
            it.setup(null)
            `qsText.forms.add(it)
        }

        add(4, 48, Argb.White, Argb.Red)
    }

    mtd loadQuickStyles()
    {
        loadQuickStyleArrow()
        loadQuickStyleShape()
        loadQuickStyleLine()
        loadQuickStylePolygon()
        loadQuickStyleText()
    }

    mtd updateSelection()
    {
        if !currentStyle
            return
        done := false
        visit it, i: currentStyle.forms
        {
            type := @kindof(it)
            btn  := cast(*IconButton) layout.childs[i]
            switch type
            {
            case FormArrow:
                btn.isChecked = it.isSame(&main.mdlFormArrow)
            case FormShape:
                btn.isChecked = it.isSame(&main.mdlFormShape)
            case FormLine:
                btn.isChecked = it.isSame(&main.mdlFormLine)
            case FormPolygon:
                btn.isChecked = it.isSame(&main.mdlFormPolygon)
            case FormText:
                btn.isChecked = it.isSame(&main.mdlFormText)
            }

            if btn.isChecked
                done = true
        }

        curMdl := cast(*IconButton) layout.childs.back()
        curMdl.show(!done)
        if !done
            curMdl.isChecked = true

        invalidate()
    }

    mtd updateQuickStyles()
    {
        func paintForm(painter: *Painter, btnp: *QuickButton)
        {
            painter.setParams(.Antialiased)
            sel  := cast(*BaseForm) @dataof(btnp.formItf)
            s    := cast(s32) BaseForm.adaptSize(sel.borderSize / 2, MinBorderSize, MaxBorderSize, 1, true)
            rect := Math.Rectangle{8 + s, 8 + s, 48 - 2*s, 48 - 2*s}

            switch @kindof(btnp.formItf)
            {
            case FormPolygon:
                poly := cast(*FormPolygon) @dataof(btnp.formItf)
                poly.setDefaultShape()
            case FormLine, FormArrow:
                line := cast(*FormLine) @dataof(btnp.formItf)
                xo   := FormLine.drawCapOffset(line.beginCap, line.borderSize, line.startSize * 0.5)
                yo   := FormLine.drawCapOffset(line.endCap, line.borderSize, line.endSize * 0.5)
                rect.moveLeft(xo)
                rect.moveTop(xo)
                rect.moveRight(-yo)
                rect.moveBottom(-yo)
            }

            btnp.formItf.setBaseForm(acast rect.x, acast rect.y, acast rect.width, acast rect.height)
            Capture.paintForm(btnp.getApp(), painter, @{0, 0, 64, 64}, btnp.formItf, zoom: 1, quickMode: true)
        }

        switch main.editMode
        {
        case Arrow:
            currentStyle = qsArrow
        case Shape:
            currentStyle = qsShape
        case Line:
            currentStyle = qsLine
        case Polygon:
            currentStyle = qsPolygon
        case Text:
            currentStyle = qsText
        default:
            currentStyle = null
        }

        while layout.childs.count
            layout.childs[0].destroyNow()
        if !currentStyle
            return

        visit it: currentStyle.forms
        {
            btn := Wnd.create'QuickButton(layout, null, @{0, 0, 64, 64})
            btn.iconBtnFlags |= .Checkable
            btn.setForm(.RoundSquare)
            btn.formItf = it

            btn.sigPaintIcon = @(btn, bc, pos) {
                paintForm(bc.painter, cast(*QuickButton) btn)
            }

            btn.sigCheckChanged += @|self, it|(btn) {
                if !btn.isChecked
                    return
                type := @kindof(it)
                var mdlValue: *BaseForm
                switch type
                {
                case FormShape:
                    b := cast(*FormShape) @dataof(it)
                    self.main.mdlFormShape = dref b
                    mdlValue = Memory.new'FormShape()
                    it.copy(mdlValue, b)
                case FormLine:
                    b := cast(*FormLine) @dataof(it)
                    self.main.mdlFormLine = dref b
                    mdlValue = Memory.new'FormLine()
                    it.copy(mdlValue, b)
                case FormPolygon:
                    b := cast(*FormPolygon) @dataof(it)
                    self.main.mdlFormPolygon = dref b
                    mdlValue = Memory.new'FormPolygon()
                    it.copy(mdlValue, b)
                case FormArrow:
                    b := cast(*FormArrow) @dataof(it)
                    self.main.mdlFormArrow = dref b
                    mdlValue = Memory.new'FormArrow()
                    it.copy(mdlValue, b)
                case FormText:
                    b := cast(*FormText) @dataof(it)
                    self.main.mdlFormText = dref b
                    mdlValue = Memory.new'FormText()
                    it.copy(mdlValue, b)
                }

                capture := self.main.getCapture()
                if capture.selection.count
                {
                    undo := capture.newUndo(.SetFormFromModel)
                    undo.mdlType = type
                    undo.mdlValue = mdlValue
                    capture.pushUndo(undo)
                }

                self.updateSelection()
                self.main.updateState()
            }
        }

        // The button with the current model
        btn := cast(*QuickButton) Wnd.create'QuickButton(layout, null, @{0, 0, 64, 64})
        btn.setForm(.RoundSquare)
        btn.style.addStyleSheetColors("btnIcon_Bk          0x0")
        btn.style.addStyleSheetColors("btnIcon_HotBk       0x0")
        btn.style.addStyleSheetColors("btnIcon_PressedBk   0x0")

        #[Swag.Complete]
        switch main.editMode
        {
        case Arrow:
            btn.formItf = cast(IForm) main.mdlFormArrow
        case Shape:
            btn.formItf = cast(IForm) main.mdlFormShape
        case Line:
            btn.formItf = cast(IForm) main.mdlFormLine
        case Polygon:
            btn.formItf = cast(IForm) main.mdlFormPolygon
        case Text:
            btn.formItf = cast(IForm) main.mdlFormText
        }

        btn.sigPaintIcon = @(btn, bc, pos) {
            paintForm(bc.painter, cast(*QuickButton) btn)
            rect := btn.getClientRect()
            rect.inflate(-2, -2)
            bc.painter.setParams(.Antialiased)
            bc.painter.drawRoundRect(rect, 5, 5, Pen.createDash(.Dash, Argb.White, 2))
        }

        // Button to add a new style
        ico  := getTheme().getIcon24(ThemeIcons24.Plus)
        btn1 := IconButton.create(btn, ico, @{64 - 26, 2, 24, 24})
        btn1.style.addStyleSheetColors("btnIcon_Bk          $hilight")
        btn1.style.addStyleSheetColors("btnIcon_HotBk       $hilightLight")
        btn1.style.addStyleSheetColors("btnIcon_PressedBk   $hilightDark")
        btn1.setForm(.Round)
        btn1.sigPressed += @|self, main|(btn) {
            #[Swag.Complete]
            switch main.editMode
            {
            case Arrow:
                mdl := Memory.new'FormArrow()
                mdl.IForm.copy(mdl, &main.mdlFormArrow)
                self.currentStyle.forms.add(mdl)
            case Shape:
                mdl := Memory.new'FormShape()
                mdl.IForm.copy(mdl, &main.mdlFormShape)
                self.currentStyle.forms.add(mdl)
            case Line:
                mdl := Memory.new'FormLine()
                mdl.IForm.copy(mdl, &main.mdlFormLine)
                self.currentStyle.forms.add(mdl)
            case Polygon:
                mdl := Memory.new'FormPolygon()
                mdl.IForm.copy(mdl, &main.mdlFormPolygon)
                self.currentStyle.forms.add(mdl)
            case Text:
                mdl := Memory.new'FormText()
                mdl.IForm.copy(mdl, &main.mdlFormText)
                self.currentStyle.forms.add(mdl)
            }
            self.updateQuickStyles()
        }

        updateSelection()
        layout.computeLayout()
    }
}