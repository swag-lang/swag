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
                form := cast(*FormArrow) @dataof(it)
                btn.isChecked = form.isSameMdl(main.mdlFormArrow)
                btn.invalidate()
            }
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
                Capture.paintForm(btn.getApp(), bc.painter, @{0, 0, 64, 64}, it, 1)
            }

            btn.sigCheckChanged += @|self, it|(btn) {
                if !btn.isChecked
                    return
                type := @kindof(it)
                switch type
                {
                case FormArrow:
                    b := cast(*FormArrow) @dataof(it)
                    self.main.mdlFormArrow = dref b
                }

                capture := self.main.getCapture()
                if capture
                    capture.selection.clear()
                self.updateSelection()
                self.main.updateState()
            }
        }

        updateSelection()
        layout.computeLayout()
    }
}