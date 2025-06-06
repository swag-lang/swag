struct QuickStyle
{
    forms: Array'IForm
}

impl QuickStyle
{
    mtd opDrop()
    {
        clear()
    }

    func deleteOne(itf: IForm)
    {
        Memory.delete(@dataof(itf), @kindof(itf))
    }

    mtd remove(itf: IForm)
    {
        deleteOne(itf)
        forms.remove(itf)
    }

    mtd clear()
    {
        foreach i in forms:
            deleteOne(i)
        forms.clear()
    }
}

struct QuickButton
{
    using iconBtn:     IconButton
    formItf:           IForm
}

struct QuickWnd
{
    using scrollWnd:     ScrollWnd

    layout:              *WrapLayoutCtrl
    main:                *MainWnd

    qsArrow:             QuickStyle
    qsShape:             QuickStyle
    qsImage:             QuickStyle
    qsStep:              QuickStyle
    qsLine:              QuickStyle
    qsPolygon:           QuickStyle
    qsText:              QuickStyle
    qsFavorites:         QuickStyle
    qsDraw:              QuickStyle
    qsHighlight:         QuickStyle
    qsPixSel:            QuickStyle

    currentStyle:        *QuickStyle
}

impl IWnd for QuickWnd
{
    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        if evt.kind == .MousePressed and evt.button == .Right
        {
            btnPopup(null, evt.surfacePos)
            evt.accepted = true
            return
        }

        scrollWnd.IWnd.onMouseEvent(evt)
    }

    mtd impl onSerializeStateEvent(evt: *SerializeStateEvent)
    {
        catch
        {
            with evt.ser
            {
                .serialize("QuickWnd.qsArrow", &qsArrow)
                .serialize("QuickWnd.qsShape", &qsShape)
                .serialize("QuickWnd.qsImage", &qsImage)
                .serialize("QuickWnd.qsStep", &qsStep)
                .serialize("QuickWnd.qsLine", &qsLine)
                .serialize("QuickWnd.qsPolygon", &qsPolygon)
                .serialize("QuickWnd.qsText", &qsText)
                .serialize("QuickWnd.qsFavorites", &qsFavorites)
                .serialize("QuickWnd.qsDraw", &qsDraw)
                .serialize("QuickWnd.qsHighlight", &qsHighlight)
                .serialize("QuickWnd.qsPixSel", &qsPixSel)
            }
        }
    }
}

impl QuickWnd
{
    mtd setDefault()
    {
        if qsArrow.forms.isEmpty():
            defaultQuickStyleArrow()
        if qsShape.forms.isEmpty():
            defaultQuickStyleShape()
        if qsImage.forms.isEmpty():
            defaultQuickStyleImage()
        if qsStep.forms.isEmpty():
            defaultQuickStyleStep()
        if qsLine.forms.isEmpty():
            defaultQuickStyleLine()
        if qsPolygon.forms.isEmpty():
            defaultQuickStylePolygon()
        if qsText.forms.isEmpty():
            defaultQuickStyleText()
        if qsDraw.forms.isEmpty():
            defaultQuickStyleDraw()
        if qsHighlight.forms.isEmpty():
            defaultQuickStyleHighlight()
        if qsPixSel.forms.isEmpty():
            defaultQuickStylePixSel()

        updateQuickStyles()

        main.updateModel(qsArrow.forms[0])
        main.updateModel(qsLine.forms[0])
        main.updateModel(qsShape.forms[0])
        main.updateModel(qsImage.forms[0])
        main.updateModel(qsStep.forms[0])
        main.updateModel(qsPolygon.forms[0])
        main.updateModel(qsText.forms[0])
        main.updateModel(qsDraw.forms[0])
        updateSelection()
    }

    mtd btnPopup(btn: #null *Button, pos: Math.Point)
    {
        let menu = MenuCtrl.createPopup(self)
        menu.addItem("Remove Quick Style", "0", disabled: !btn)
        menu.addSeparator()
        menu.addItem(CmdIdResetQuickStyle)
        let res = menu.doModal(pos, autoDestroy: true)
        if res == "0" and btn
        {
            let qbtn = cast(*QuickButton) btn
            currentStyle.remove(qbtn.formItf)
            qbtn.destroy()
            updateQuickStyles()
        }
    }

    func create(parent: *Wnd)->*QuickWnd
    {
        with let quick = Wnd.create'QuickWnd(parent, {0, 0, 0, 300})
        {
            .backgroundStyle = .Window
            .scrollWndFlags |= .ForceVertical | .SmallBar | .TransparentBar

            .layout         = WrapLayoutCtrl.create(quick)
            .layout.padding = 6
            .layout.spacing = 6
            .setView(quick.layout)
        }

        return quick
    }

    mtd defaultQuickStyleArrow()
    {
        mtd add(startSize, borderSize, endSize: f32, color: Color, startCap = LineCapStyle.ArrowAnchor, dash = DashStyle.None)
        {
            let form = Memory.new'FormArrow()
            form.startSize  = startSize
            form.borderSize = borderSize
            form.endSize    = endSize
            form.dashStyle  = dash
            if startSize != 0:
                form.beginCap = startCap
            if endSize != 0:
                form.endCap = .ArrowAnchor
            form.borderColor = color
            let it = cast(IForm) form
            it.setup(null)
            qsArrow.forms.add(it)
        }

        const BorderSize = 10.0
        const ArrowSize  = 5.0

        add(0, BorderSize, ArrowSize, ColorRed)
        add(0, BorderSize, ArrowSize, ColorGreen)
        add(0, BorderSize, ArrowSize, ColorBlue)
        add(0, BorderSize, ArrowSize, ColorYellow)

        add(ArrowSize, BorderSize, ArrowSize, ColorRed)
        add(ArrowSize, BorderSize, ArrowSize, ColorGreen)
        add(ArrowSize, BorderSize, ArrowSize, ColorBlue)
        add(ArrowSize, BorderSize, ArrowSize, ColorYellow)

        add(ArrowSize * 0.5, BorderSize, ArrowSize, ColorRed, .RoundAnchor, .Dot)
        add(ArrowSize * 0.5, BorderSize, ArrowSize, ColorGreen, .RoundAnchor, .Dot)
        add(ArrowSize * 0.5, BorderSize, ArrowSize, ColorBlue, .RoundAnchor, .Dot)
        add(ArrowSize * 0.5, BorderSize, ArrowSize, ColorYellow, .RoundAnchor, .Dot)
    }

    mtd defaultQuickStyleLine()
    {
        mtd add(borderSize: f32, color: Color)
        {
            let form = Memory.new'FormLine()
            form.borderSize  = borderSize
            form.borderColor = color
            let it = cast(IForm) form
            it.setup(null)
            qsLine.forms.add(it)
        }

        const BorderSize = 10.0

        add(BorderSize, ColorRed)
        add(BorderSize, ColorGreen)
        add(BorderSize, ColorBlue)
        add(BorderSize, ColorYellow)
        add(BorderSize, Argb.White)
        add(BorderSize, Argb.Gray)
        add(BorderSize, Argb.Black)
    }

    mtd defaultQuickStyleShape()
    {
        mtd add(borderSize: f32, kind: FormShapeKind, color: Color, backColor: Color)
        {
            let form = Memory.new'FormShape()
            form.borderSize  = borderSize
            form.borderColor = color
            form.backColor   = backColor
            form.kind        = kind
            let it = cast(IForm) form
            it.setup(null)
            qsShape.forms.add(it)
        }

        const BorderSize = 10.0

        add(BorderSize, .RoundRectangle, ColorRed, Argb.Zero)
        add(BorderSize, .RoundRectangle, ColorGreen, Argb.Zero)
        add(BorderSize, .RoundRectangle, ColorBlue, Argb.Zero)
        add(BorderSize, .RoundRectangle, ColorYellow, Argb.Zero)

        add(BorderSize, .Rectangle, ColorRed, Argb.Zero)
        add(BorderSize, .Rectangle, ColorGreen, Argb.Zero)
        add(BorderSize, .Rectangle, ColorBlue, Argb.Zero)
        add(BorderSize, .Rectangle, ColorYellow, Argb.Zero)

        add(BorderSize, .Ellipse, ColorRed, Argb.Zero)
        add(BorderSize, .Ellipse, ColorGreen, Argb.Zero)
        add(BorderSize, .Ellipse, ColorBlue, Argb.Zero)
        add(BorderSize, .Ellipse, ColorYellow, Argb.Zero)

        add(BorderSize, .Rectangle, Argb.White, Argb.Zero)
        add(BorderSize, .Rectangle, Argb.Black, Argb.Zero)

        add(0, .Rectangle, Argb.Zero, Argb.White)
        add(0, .Rectangle, Argb.Zero, Argb.Black)
    }

    mtd defaultQuickStyleImage()
    {
        mtd add(borderSize: f32, kind: FormImageKind, color: Color)
        {
            let form = Memory.new'FormImage()
            form.borderSize  = borderSize
            form.borderColor = color
            form.backColor   = Argb.Gray
            form.kind        = kind
            if borderSize == 0:
                form.paintShadow = false
            let it = cast(IForm) form
            it.setup(null)
            qsImage.forms.add(it)
        }

        add(2, .Image, Argb.White)
        add(2, .Zoom, Argb.White)
        add(0, .Blur, Argb.Zero)
        add(0, .Pixelate, Argb.Zero)
        add(0, .Capture, Argb.Zero)
    }

    mtd defaultQuickStyleStep()
    {
        mtd add(kind: FormStepKind, color: Color, backColor: Color)
        {
            let form = Memory.new'FormStep()
            form.borderColor = color
            form.backColor   = backColor
            form.kind        = kind
            let it = cast(IForm) form
            it.setup(null)
            qsStep.forms.add(it)
        }

        add(.Ellipse, Argb.White, ColorRed)
        add(.Ellipse, Argb.White, ColorGreen)
        add(.Ellipse, Argb.White, ColorBlue)
        add(.Ellipse, Argb.White, ColorYellow)

        add(.RoundRectangle, Argb.White, ColorRed)
        add(.RoundRectangle, Argb.White, ColorGreen)
        add(.RoundRectangle, Argb.White, ColorBlue)
        add(.RoundRectangle, Argb.White, ColorYellow)
    }

    mtd defaultQuickStylePolygon()
    {
        mtd add(borderSize: f32, color: Color, bezier = false)
        {
            let form = Memory.new'FormPolygon()
            form.borderSize  = borderSize
            form.borderColor = color
            form.backColor   = 0
            form.bezier      = bezier
            let it = cast(IForm) form
            it.setup(null)
            qsPolygon.forms.add(it)
        }

        const BorderSize = 5.0

        add(BorderSize, ColorRed)
        add(BorderSize, ColorGreen)
        add(BorderSize, ColorBlue)
        add(BorderSize, ColorYellow)

        add(BorderSize, ColorRed, true)
        add(BorderSize, ColorGreen, true)
        add(BorderSize, ColorBlue, true)
        add(BorderSize, ColorYellow, true)

        add(BorderSize, Argb.White)
        add(BorderSize, Argb.Black)
    }

    mtd defaultQuickStyleText()
    {
        mtd add(borderSize, fontSize: f32, bdColor, bkColor: Color)
        {
            let form = Memory.new'FormText()
            form.borderSize  = borderSize
            form.borderColor = bdColor
            form.backColor   = bkColor
            form.fontSize    = fontSize
            form.text        = "A"
            let it = cast(IForm) form
            it.setup(null)
            qsText.forms.add(it)
        }

        add(4, 24, Argb.White, ColorRed)
        add(4, 24, Argb.White, ColorGreen)
        add(4, 48, Argb.White, ColorBlue)
        add(4, 48, Argb.White, ColorYellow)

        add(4, 72, Argb.White, ColorRed)
        add(4, 72, Argb.White, ColorGreen)
        add(4, 72, Argb.White, ColorBlue)
        add(4, 72, Argb.White, ColorYellow)

        add(4, 48, Argb.Zero, Argb.White)
        add(4, 48, Argb.Zero, Argb.Gray)
        add(4, 48, Argb.Zero, Argb.Black)

        add(4, 72, Argb.Zero, Argb.White)
        add(4, 72, Argb.Zero, Argb.Gray)
        add(4, 72, Argb.Zero, Argb.Black)
    }

    mtd defaultQuickStyleDraw()
    {
        mtd add(borderSize: f32, color: Color)
        {
            let form = Memory.new'FormDraw()
            form.borderSize  = borderSize
            form.borderColor = color
            let it = cast(IForm) form
            it.setup(null)
            qsDraw.forms.add(it)
        }

        const BorderSize = 10.0
        add(BorderSize, ColorRed)
        add(BorderSize, ColorGreen)
        add(BorderSize, ColorBlue)
        add(BorderSize, ColorYellow)

        add(BorderSize, Argb.White)
        add(BorderSize, Argb.Gray)
        add(BorderSize, Argb.Black)
    }

    mtd defaultQuickStyleHighlight()
    {
        mtd addDraw(borderSize: f32, color: Color)
        {
            let form = Memory.new'FormDraw()
            form.borderSize  = borderSize
            form.borderColor = color
            form.alpha       = 30
            form.subKind     = .Highlight
            let it = cast(IForm) form
            it.setup(null)
            qsHighlight.forms.add(it)
        }

        mtd addShape(kind: FormShapeKind, backColor: Color)
        {
            let form = Memory.new'FormShape()
            form.borderSize  = 0
            form.borderColor = Argb.Zero
            form.backColor   = backColor
            form.alpha       = 30
            form.kind        = kind
            form.subKind     = .Highlight
            let it = cast(IForm) form
            it.setup(null)
            qsHighlight.forms.add(it)
        }

        mtd addPoly(backColor: Color)
        {
            let form = Memory.new'FormPolygon()
            form.borderSize  = 0
            form.borderColor = Argb.Zero
            form.backColor   = backColor
            form.alpha       = 30
            form.subKind     = .Highlight
            let it = cast(IForm) form
            it.setup(null)
            qsHighlight.forms.add(it)
        }

        addShape(.Rectangle, ColorRed)
        addShape(.Rectangle, ColorGreen)
        addShape(.Rectangle, ColorBlue)
        addShape(.Rectangle, ColorYellow)

        const BorderSize = 20.0
        addDraw(BorderSize, ColorRed)
        addDraw(BorderSize, ColorGreen)
        addDraw(BorderSize, ColorBlue)
        addDraw(BorderSize, ColorYellow)

        addPoly(ColorRed)
        addPoly(ColorGreen)
        addPoly(ColorBlue)
        addPoly(ColorYellow)
    }

    mtd defaultQuickStylePixSel()
    {
        mtd addDraw()
        {
            let form = Memory.new'FormDraw()
            form.subKind     = .PixSel
            form.paintShadow = false
            let it = cast(IForm) form
            it.setup(null)
            qsPixSel.forms.add(it)
        }

        mtd addShape(kind: FormShapeKind)
        {
            let form = Memory.new'FormShape()
            form.kind        = kind
            form.subKind     = .PixSel
            form.paintShadow = false
            let it = cast(IForm) form
            it.setup(null)
            qsPixSel.forms.add(it)
        }

        mtd addPoly()
        {
            let form = Memory.new'FormPolygon()
            form.subKind     = .PixSel
            form.paintShadow = false
            let it = cast(IForm) form
            it.setup(null)
            qsPixSel.forms.add(it)
        }

        addShape(.Rectangle)
        addShape(.Ellipse)
        addPoly()
        addDraw()
    }

    mtd updateSelection()
    {
        if !currentStyle:
            return
        var done = false
        let mode = main.getEditMode()

        var firstVisible = 0's32
        foreach c in layout.childs
        {
            if c.isVisible():
                break
            firstVisible += 1
        }

        foreach it, i in currentStyle.forms
        {
            let btn = cast(*IconButton) layout.childs[i + firstVisible]

            #[Swag.Complete]
            switch it
            {
            case FormArrow:
                btn.isChecked = mode == .Arrow and it.isSame(&main.mdlFormArrow)
            case FormShape:
                btn.isChecked = mode == .Shape and it.isSame(&main.mdlFormShape)
            case FormImage:
                btn.isChecked = mode == .Image and it.isSame(&main.mdlFormImage)
            case FormStep:
                btn.isChecked = mode == .Step and it.isSame(&main.mdlFormStep)
            case FormLine:
                btn.isChecked = mode == .Line and it.isSame(&main.mdlFormLine)
            case FormPolygon:
                btn.isChecked = mode == .Polygon and it.isSame(&main.mdlFormPolygon)
            case FormText:
                btn.isChecked = mode == .Text and it.isSame(&main.mdlFormText)
            case FormDraw:
                btn.isChecked = mode == .Draw and it.isSame(&main.mdlFormDraw)
            }

            if btn.isChecked:
                done = true
        }

        let curMdl = cast(*IconButton) layout.childs.back()
        if curMdl.userData
        {
            curMdl.show(!done)
            if !done:
                curMdl.isChecked = true
            layout.computeLayout()
            setScrollSize(0, layout.resultHeight)
        }

        invalidate()
    }

    mtd resetQuickStyles()
    {
        if !currentStyle:
            return

        let res = MessageDlg.confirm(surface, "This will reset the styles to the default values.")
        if res != MessageDlg.BtnYes:
            return

        currentStyle.clear()

        #[Swag.Complete]
        switch main.editMode
        {
        case Arrow:
            defaultQuickStyleArrow()
        case Shape:
            defaultQuickStyleShape()
        case Image:
            defaultQuickStyleImage()
        case Step:
            defaultQuickStyleStep()
        case Line:
            defaultQuickStyleLine()
        case Polygon:
            defaultQuickStylePolygon()
        case Text:
            defaultQuickStyleText()
        case Draw:
            defaultQuickStyleDraw()
        case Highlight:
            defaultQuickStyleHighlight()
        case PixSel:
            defaultQuickStylePixSel()
        case Favorites:
            break
        case Selection:
            break
        }

        updateQuickStyles()
    }

    mtd updateQuickStyles()
    {
        func paintForm(painter: *Painter, btnp: *QuickButton)
        {
            var cxt: FormPaintContext
            cxt.z         = 1
            cxt.quickMode = true

            painter.setParams(.Antialiased)
            let sel  = cast(*BaseForm) @dataof(btnp.formItf)
            let s    = cast(s32) BaseForm.adaptSize(sel.borderSize / 2, MinBorderSize, MaxBorderSize, cxt)
            var rect = Math.Rectangle{8 + s, 8 + s, 48 - 2 * s, 48 - 2 * s}

            var title = ""

            switch btnp.formItf
            {
            case FormPolygon:
                let poly = cast(*FormPolygon) @dataof(btnp.formItf)
                poly.setDefaultShape()
            case FormDraw:
                let poly = cast(*FormDraw) @dataof(btnp.formItf)
                poly.setDefaultShape()
            case FormText:
                let txt = cast(*FormText) @dataof(btnp.formItf)
                txt.text = "A"
            case FormImage:
                let img = cast(*FormImage) @dataof(btnp.formItf)
                switch img.kind
                {
                case Image:
                    title = "image"
                case Blur:
                    title = "blur"
                case Pixelate:
                    title = "pixelate"
                case Zoom:
                    title = "zoom"
                case Capture:
                    title = "capture"
                }
            }

            btnp.formItf.setBaseForm(cast() rect.x, cast() rect.y, cast() rect.width, cast() rect.height)
            Capture.paintForm(painter, {}, {0, 0, 64, 64}, btnp.formItf, zoom: 1, quickMode: true)

            if @countof(title):
                painter.drawStringCenter(rect.horzCenter(), rect.bottom(), title, btnp.getFont().regular, Argb.White)
        }

        let edMode = main.editMode == .Selection ? main.getEditMode() : main.editMode
        #[Swag.Complete]
        switch edMode
        {
        case Arrow:
            currentStyle = &qsArrow
        case Shape:
            currentStyle = &qsShape
        case Image:
            currentStyle = &qsImage
        case Step:
            currentStyle = &qsStep
        case Line:
            currentStyle = &qsLine
        case Polygon:
            currentStyle = &qsPolygon
        case Text:
            currentStyle = &qsText
        case Favorites:
            currentStyle = &qsFavorites
        case Draw:
            currentStyle = &qsDraw
        case Highlight:
            currentStyle = &qsHighlight
        case PixSel:
            currentStyle = &qsPixSel
        case Selection:
            return
        }

        foreach c in layout.childs:
            c.destroy()
        if !currentStyle:
            return

        foreach it in currentStyle.forms
        {
            let btn = Wnd.create'QuickButton(layout, null, {0, 0, 64, 64})
            btn.iconBtnFlags.add(.Checkable)
            btn.setForm(.RoundSquare)
            btn.formItf = it

            btn.sigPaintIcon = func(btn, bc, pos)
            {
                paintForm(bc.painter, cast(*QuickButton) btn)
            }

            btn.sigRightPressed += func|self|(btn)
            {
                var rect = btn.getSurfaceRect()
                self.btnPopup(btn, rect.center())
            }

            btn.sigCheckChanged += func|self, it|(btn)
            {
                if !btn.isChecked:
                    return
                self.main.updateModel(it)

                var capture = assume self.main.getCapture()
                if capture.selection.count
                {
                    let mdlValue = MainWnd.newFormFromForm(it)
                    let undo     = capture.newUndo(.SetFormFromModel)
                    undo.mdlType  = @kindof(it)
                    undo.mdlValue = mdlValue
                    capture.pushUndo(undo)
                }
                else
                {
                    capture.selection.clear()
                }

                self.updateSelection()
                self.main.updateState()
            }
        }

        let formItf     = main.editModeToModel(main.getEditMode())
        let baseForm    = cast(*BaseForm) @dataof(formItf)
        var canAddModel = false
        if baseForm
        {
            canAddModel = true
            if main.editMode == .Highlight
            {
                if baseForm.subKind != .Highlight:
                    canAddModel = false
            }
            elif main.editMode == .PixSel
            {
                canAddModel = false
            }
            elif baseForm.subKind == .Highlight or baseForm.subKind == .PixSel
            {
                canAddModel = false
            }
        }

        if canAddModel
        {
            let btn = Wnd.create'QuickButton(layout, null, {0, 0, 64, 64})
            btn.userData = cast(*void) 1'u64 // This is the model button
            btn.formItf  = formItf
            btn.setForm(.RoundSquare)
            btn.style.addStyleSheetColors("btnIcon_Bk          0x0")
            btn.style.addStyleSheetColors("btnIcon_HotBk       0x0")
            btn.style.addStyleSheetColors("btnIcon_PressedBk   0x0")
            layout.sendResizeEvent()

            btn.sigPaintIcon = func(btn, bc, pos)
            {
                paintForm(bc.painter, cast(*QuickButton) btn)
                var rect = btn.getClientRect()
                rect.inflate(-2, -2)
                bc.painter.setParams(.Antialiased)
                bc.painter.drawRoundRect(rect, 5, 5, Pen.createDash(.Dash, Argb.White, 2))
            }

            // Button to add a new style
            let ico  = getTheme().getIcon24(.Plus)
            let btn1 = IconButton.create(btn, ico, {64 - 26, 2, 24, 24})
            btn1.style.addStyleSheetColors("btnIcon_Bk          $hilight")
            btn1.style.addStyleSheetColors("btnIcon_HotBk       $hilightLight")
            btn1.style.addStyleSheetColors("btnIcon_PressedBk   $hilightDark")
            btn1.setForm(.Round)

            layout.sendResizeEvent()
            setScrollSize(0, layout.resultHeight)

            btn1.sigPressed += func|self, main|(btn)
            {
                let mdl = self.main.newFormFromModel(main.getEditMode())
                if mdl
                {
                    self.currentStyle.forms.add(mdl)
                    self.updateQuickStyles()
                }
            }
        }

        updateSelection()
        layout.computeLayout()
    }
}
