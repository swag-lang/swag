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
        quick.setView(quick.layout)
        quick.loadQuickStyles()
        return quick
    }

    mtd loadQuickStyleArrow()
    {
        qsArrow = Memory.new'QuickStyle()

        {
            form := Memory.new'FormArrow()
            form.borderSize = 5
            form.endSize = 10
            form.endShape = .Arrow
            form.borderColor = Argb.Red
            it := cast(IForm) form
            it.setup(null)
            qsArrow.forms.add(it)
        }
        {
            form := Memory.new'FormArrow()
            form.borderSize = 5
            form.endSize, form.startSize = 10
            form.startShape = .Arrow
            form.endShape = .Arrow
            form.borderColor = Argb.Green
            it := cast(IForm) form
            it.setup(null)
            qsArrow.forms.add(it)
        }
    }

    mtd loadQuickStyleShape()
    {
        qsShape = Memory.new'QuickStyle()
    }

    mtd loadQuickStyleLine()
    {
        qsLine = Memory.new'QuickStyle()
    }

    mtd loadQuickStylePolygon()
    {
        qsPolygon = Memory.new'QuickStyle()
    }

    mtd loadQuickStyles()
    {
        loadQuickStyleArrow()
        loadQuickStyleShape()
        loadQuickStyleLine()
        loadQuickStylePolygon()
    }

    mtd updateSelection()
    {
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
            }

            btn.invalidate()
        }
    }

    mtd updateQuickStyles()
    {
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
                bc.painter.setParams(.Antialiased)
                btnp := cast(*QuickButton) btn
                btnp.formItf.setBaseForm(8, 8, 48, 48)
                Capture.paintForm(btn.getApp(), bc.painter, @{0, 0, 64, 64}, btnp.formItf, zoom: 1, blurRadius: 4.0)
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
                    it.copy(mdlValue, &self.main.mdlFormShape)
                case FormLine:
                    b := cast(*FormLine) @dataof(it)
                    self.main.mdlFormLine = dref b
                    mdlValue = Memory.new'FormLine()
                    it.copy(mdlValue, &self.main.mdlFormLine)
                case FormPolygon:
                    b := cast(*FormPolygon) @dataof(it)
                    mdlValue = Memory.new'FormPolygon()
                    it.copy(mdlValue, &self.main.mdlFormPolygon)
                case FormArrow:
                    b := cast(*FormArrow) @dataof(it)
                    self.main.mdlFormArrow = dref b
                    mdlValue = Memory.new'FormArrow()
                    it.copy(mdlValue, &self.main.mdlFormArrow)
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
        btn.iconBtnFlags |= .Checkable
        btn.setForm(.RoundSquare)

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
        }

        btn.sigPaintIcon = @(btn, bc, pos) {
            bc.painter.setParams(.Antialiased)
            btnp := cast(*QuickButton) btn
            btnp.formItf.setBaseForm(8, 8, 48, 48)
            Capture.paintForm(btn.getApp(), bc.painter, @{0, 0, 64, 64}, btnp.formItf, zoom: 1, blurRadius: 4.0)

            rect := btn.getClientRect()
            rect.inflate(-2, -2)
            bc.painter.setParams(.Antialiased)
            bc.painter.drawRoundRect(rect, 5, 5, Pen.createDash(.Dash, Argb.White, 2))
        }

        updateSelection()
        layout.computeLayout()
    }
}