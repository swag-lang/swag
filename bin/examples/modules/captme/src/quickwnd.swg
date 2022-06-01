using Core, Gui, Pixel

struct QuickButton
{
    using iconBtn: IconButton
    form: IForm
}

struct QuickWnd
{
    using scrollWnd: ScrollWnd

    layout:         *WrapLayoutCtrl
    main:           *MainWnd
    qsArrow:        *QuickStyle
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

    mtd loadQuickStyles()
    {
        loadQuickStyleArrow()
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
            btn.form = it

            btn.sigPaintIcon = @|it|(btn, bc, pos) {
                bc.painter.setParams(.Antialiased)
                it.setBaseForm(8, 8, 48, 48)
                Capture.paintForm(btn.getApp(), bc.painter, @{0, 0, 64, 64}, it, zoom: 1, blurRadius: 4.0)
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
                    mdlValue = &self.main.mdlFormShape
                case FormLine:
                    b := cast(*FormLine) @dataof(it)
                    self.main.mdlFormLine = dref b
                    mdlValue = &self.main.mdlFormLine
                case FormPolygon:
                    b := cast(*FormPolygon) @dataof(it)
                    self.main.mdlFormPolygon = dref b
                    mdlValue = &self.main.mdlFormPolygon
                case FormArrow:
                    b := cast(*FormArrow) @dataof(it)
                    self.main.mdlFormArrow = dref b
                    mdlValue = &self.main.mdlFormArrow
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

        updateSelection()
        layout.computeLayout()
    }
}