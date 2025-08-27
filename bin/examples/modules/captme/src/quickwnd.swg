#global marked
struct QuickStyle
{
    forms: Array'IForm
}

impl QuickStyle
{
    mtd opDrop()
    {
        me.clear()
    }

    func deleteOne(itf: IForm)
    {
        Memory.delete(@dataof(itf), @kindof(itf))
    }

    mtd remove(itf: IForm)
    {
        deleteOne(itf)
        me.forms.remove(itf)
    }

    mtd clear()
    {
        foreach i in me.forms:
            deleteOne(i)
        me.forms.clear()
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
            me.btnPopup(null, evt.surfacePos)
            evt.accepted = true
            return
        }

        me.scrollWnd.IWnd.onMouseEvent(evt)
    }

    mtd impl onSerializeStateEvent(evt: *SerializeStateEvent)
    {
        catch
        {
            with evt.ser
            {
                .serialize("QuickWnd.qsArrow", &me.qsArrow)
                .serialize("QuickWnd.qsShape", &me.qsShape)
                .serialize("QuickWnd.qsImage", &me.qsImage)
                .serialize("QuickWnd.qsStep", &me.qsStep)
                .serialize("QuickWnd.qsLine", &me.qsLine)
                .serialize("QuickWnd.qsPolygon", &me.qsPolygon)
                .serialize("QuickWnd.qsText", &me.qsText)
                .serialize("QuickWnd.qsFavorites", &me.qsFavorites)
                .serialize("QuickWnd.qsDraw", &me.qsDraw)
                .serialize("QuickWnd.qsHighlight", &me.qsHighlight)
                .serialize("QuickWnd.qsPixSel", &me.qsPixSel)
            }
        }
    }
}

impl QuickWnd
{
    mtd setDefault()
    {
        if me.qsArrow.forms.isEmpty():
            me.defaultQuickStyleArrow()
        if me.qsShape.forms.isEmpty():
            me.defaultQuickStyleShape()
        if me.qsImage.forms.isEmpty():
            me.defaultQuickStyleImage()
        if me.qsStep.forms.isEmpty():
            me.defaultQuickStyleStep()
        if me.qsLine.forms.isEmpty():
            me.defaultQuickStyleLine()
        if me.qsPolygon.forms.isEmpty():
            me.defaultQuickStylePolygon()
        if me.qsText.forms.isEmpty():
            me.defaultQuickStyleText()
        if me.qsDraw.forms.isEmpty():
            me.defaultQuickStyleDraw()
        if me.qsHighlight.forms.isEmpty():
            me.defaultQuickStyleHighlight()
        if me.qsPixSel.forms.isEmpty():
            me.defaultQuickStylePixSel()

        me.updateQuickStyles()

        me.main.updateModel(me.qsArrow.forms[0])
        me.main.updateModel(me.qsLine.forms[0])
        me.main.updateModel(me.qsShape.forms[0])
        me.main.updateModel(me.qsImage.forms[0])
        me.main.updateModel(me.qsStep.forms[0])
        me.main.updateModel(me.qsPolygon.forms[0])
        me.main.updateModel(me.qsText.forms[0])
        me.main.updateModel(me.qsDraw.forms[0])
        me.updateSelection()
    }

    mtd btnPopup(btn: #null *Button, pos: Math.Point)
    {
        let menu = MenuCtrl.createPopup(me)
        menu.addItem("Remove Quick Style", "0", disabled: !btn)
        menu.addSeparator()
        menu.addItem(CmdIdResetQuickStyle)
        let res = menu.doModal(pos, autoDestroy: true)
        if res == "0" and btn
        {
            let qbtn = cast(*QuickButton) btn
            me.currentStyle.remove(qbtn.formItf)
            qbtn.destroy()
            me.updateQuickStyles()
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
            me.qsArrow.forms.add(it)
        }

        const BorderSize = 10.0
        const ArrowSize  = 5.0

        me.add(0, BorderSize, ArrowSize, ColorRed)
        me.add(0, BorderSize, ArrowSize, ColorGreen)
        me.add(0, BorderSize, ArrowSize, ColorBlue)
        me.add(0, BorderSize, ArrowSize, ColorYellow)

        me.add(ArrowSize, BorderSize, ArrowSize, ColorRed)
        me.add(ArrowSize, BorderSize, ArrowSize, ColorGreen)
        me.add(ArrowSize, BorderSize, ArrowSize, ColorBlue)
        me.add(ArrowSize, BorderSize, ArrowSize, ColorYellow)

        me.add(ArrowSize * 0.5, BorderSize, ArrowSize, ColorRed, .RoundAnchor, .Dot)
        me.add(ArrowSize * 0.5, BorderSize, ArrowSize, ColorGreen, .RoundAnchor, .Dot)
        me.add(ArrowSize * 0.5, BorderSize, ArrowSize, ColorBlue, .RoundAnchor, .Dot)
        me.add(ArrowSize * 0.5, BorderSize, ArrowSize, ColorYellow, .RoundAnchor, .Dot)
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
            me.qsLine.forms.add(it)
        }

        const BorderSize = 10.0

        me.add(BorderSize, ColorRed)
        me.add(BorderSize, ColorGreen)
        me.add(BorderSize, ColorBlue)
        me.add(BorderSize, ColorYellow)
        me.add(BorderSize, Argb.White)
        me.add(BorderSize, Argb.Gray)
        me.add(BorderSize, Argb.Black)
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
            me.qsShape.forms.add(it)
        }

        const BorderSize = 10.0

        me.add(BorderSize, .RoundRectangle, ColorRed, Argb.Zero)
        me.add(BorderSize, .RoundRectangle, ColorGreen, Argb.Zero)
        me.add(BorderSize, .RoundRectangle, ColorBlue, Argb.Zero)
        me.add(BorderSize, .RoundRectangle, ColorYellow, Argb.Zero)

        me.add(BorderSize, .Rectangle, ColorRed, Argb.Zero)
        me.add(BorderSize, .Rectangle, ColorGreen, Argb.Zero)
        me.add(BorderSize, .Rectangle, ColorBlue, Argb.Zero)
        me.add(BorderSize, .Rectangle, ColorYellow, Argb.Zero)

        me.add(BorderSize, .Ellipse, ColorRed, Argb.Zero)
        me.add(BorderSize, .Ellipse, ColorGreen, Argb.Zero)
        me.add(BorderSize, .Ellipse, ColorBlue, Argb.Zero)
        me.add(BorderSize, .Ellipse, ColorYellow, Argb.Zero)

        me.add(BorderSize, .Rectangle, Argb.White, Argb.Zero)
        me.add(BorderSize, .Rectangle, Argb.Black, Argb.Zero)

        me.add(0, .Rectangle, Argb.Zero, Argb.White)
        me.add(0, .Rectangle, Argb.Zero, Argb.Black)
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
            me.qsImage.forms.add(it)
        }

        me.add(2, .Image, Argb.White)
        me.add(2, .Zoom, Argb.White)
        me.add(0, .Blur, Argb.Zero)
        me.add(0, .Pixelate, Argb.Zero)
        me.add(0, .Capture, Argb.Zero)
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
            me.qsStep.forms.add(it)
        }

        me.add(.Ellipse, Argb.White, ColorRed)
        me.add(.Ellipse, Argb.White, ColorGreen)
        me.add(.Ellipse, Argb.White, ColorBlue)
        me.add(.Ellipse, Argb.White, ColorYellow)

        me.add(.RoundRectangle, Argb.White, ColorRed)
        me.add(.RoundRectangle, Argb.White, ColorGreen)
        me.add(.RoundRectangle, Argb.White, ColorBlue)
        me.add(.RoundRectangle, Argb.White, ColorYellow)
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
            me.qsPolygon.forms.add(it)
        }

        const BorderSize = 5.0

        me.add(BorderSize, ColorRed)
        me.add(BorderSize, ColorGreen)
        me.add(BorderSize, ColorBlue)
        me.add(BorderSize, ColorYellow)

        me.add(BorderSize, ColorRed, true)
        me.add(BorderSize, ColorGreen, true)
        me.add(BorderSize, ColorBlue, true)
        me.add(BorderSize, ColorYellow, true)

        me.add(BorderSize, Argb.White)
        me.add(BorderSize, Argb.Black)
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
            me.qsText.forms.add(it)
        }

        me.add(4, 24, Argb.White, ColorRed)
        me.add(4, 24, Argb.White, ColorGreen)
        me.add(4, 48, Argb.White, ColorBlue)
        me.add(4, 48, Argb.White, ColorYellow)

        me.add(4, 72, Argb.White, ColorRed)
        me.add(4, 72, Argb.White, ColorGreen)
        me.add(4, 72, Argb.White, ColorBlue)
        me.add(4, 72, Argb.White, ColorYellow)

        me.add(4, 48, Argb.Zero, Argb.White)
        me.add(4, 48, Argb.Zero, Argb.Gray)
        me.add(4, 48, Argb.Zero, Argb.Black)

        me.add(4, 72, Argb.Zero, Argb.White)
        me.add(4, 72, Argb.Zero, Argb.Gray)
        me.add(4, 72, Argb.Zero, Argb.Black)
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
            me.qsDraw.forms.add(it)
        }

        const BorderSize = 10.0
        me.add(BorderSize, ColorRed)
        me.add(BorderSize, ColorGreen)
        me.add(BorderSize, ColorBlue)
        me.add(BorderSize, ColorYellow)

        me.add(BorderSize, Argb.White)
        me.add(BorderSize, Argb.Gray)
        me.add(BorderSize, Argb.Black)
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
            me.qsHighlight.forms.add(it)
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
            me.qsHighlight.forms.add(it)
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
            me.qsHighlight.forms.add(it)
        }

        me.addShape(.Rectangle, ColorRed)
        me.addShape(.Rectangle, ColorGreen)
        me.addShape(.Rectangle, ColorBlue)
        me.addShape(.Rectangle, ColorYellow)

        const BorderSize = 20.0
        me.addDraw(BorderSize, ColorRed)
        me.addDraw(BorderSize, ColorGreen)
        me.addDraw(BorderSize, ColorBlue)
        me.addDraw(BorderSize, ColorYellow)

        me.addPoly(ColorRed)
        me.addPoly(ColorGreen)
        me.addPoly(ColorBlue)
        me.addPoly(ColorYellow)
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
            me.qsPixSel.forms.add(it)
        }

        mtd addShape(kind: FormShapeKind)
        {
            let form = Memory.new'FormShape()
            form.kind        = kind
            form.subKind     = .PixSel
            form.paintShadow = false
            let it = cast(IForm) form
            it.setup(null)
            me.qsPixSel.forms.add(it)
        }

        mtd addPoly()
        {
            let form = Memory.new'FormPolygon()
            form.subKind     = .PixSel
            form.paintShadow = false
            let it = cast(IForm) form
            it.setup(null)
            me.qsPixSel.forms.add(it)
        }

        me.addShape(.Rectangle)
        me.addShape(.Ellipse)
        me.addPoly()
        me.addDraw()
    }

    mtd updateSelection()
    {
        if !me.currentStyle:
            return
        var done = false
        let mode = me.main.getEditMode()

        var firstVisible = 0's32
        foreach c in me.layout.childs
        {
            if c.isVisible():
                break
            firstVisible += 1
        }

        foreach it, i in me.currentStyle.forms
        {
            let btn = cast(*IconButton) me.layout.childs[i + firstVisible]

            #[Swag.Complete]
            switch it
            {
            case FormArrow:
                btn.isChecked = mode == .Arrow and it.isSame(&me.main.mdlFormArrow)
            case FormShape:
                btn.isChecked = mode == .Shape and it.isSame(&me.main.mdlFormShape)
            case FormImage:
                btn.isChecked = mode == .Image and it.isSame(&me.main.mdlFormImage)
            case FormStep:
                btn.isChecked = mode == .Step and it.isSame(&me.main.mdlFormStep)
            case FormLine:
                btn.isChecked = mode == .Line and it.isSame(&me.main.mdlFormLine)
            case FormPolygon:
                btn.isChecked = mode == .Polygon and it.isSame(&me.main.mdlFormPolygon)
            case FormText:
                btn.isChecked = mode == .Text and it.isSame(&me.main.mdlFormText)
            case FormDraw:
                btn.isChecked = mode == .Draw and it.isSame(&me.main.mdlFormDraw)
            }

            if btn.isChecked:
                done = true
        }

        let curMdl = cast(*IconButton) me.layout.childs.back()
        if curMdl.userData
        {
            curMdl.show(!done)
            if !done:
                curMdl.isChecked = true
            me.layout.computeLayout()
            me.setScrollSize(0, me.layout.resultHeight)
        }

        me.invalidate()
    }

    mtd resetQuickStyles()
    {
        if !me.currentStyle:
            return

        let res = MessageDlg.confirm(me.surface, "This will reset the styles to the default values.")
        if res != MessageDlg.BtnYes:
            return

        me.currentStyle.clear()

        #[Swag.Complete]
        switch me.main.editMode
        {
        case Arrow:
            me.defaultQuickStyleArrow()
        case Shape:
            me.defaultQuickStyleShape()
        case Image:
            me.defaultQuickStyleImage()
        case Step:
            me.defaultQuickStyleStep()
        case Line:
            me.defaultQuickStyleLine()
        case Polygon:
            me.defaultQuickStylePolygon()
        case Text:
            me.defaultQuickStyleText()
        case Draw:
            me.defaultQuickStyleDraw()
        case Highlight:
            me.defaultQuickStyleHighlight()
        case PixSel:
            me.defaultQuickStylePixSel()
        case Favorites:
            break
        case Selection:
            break
        }

        me.updateQuickStyles()
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

        let edMode = me.main.editMode == .Selection ? me.main.getEditMode() : me.main.editMode
        #[Swag.Complete]
        switch edMode
        {
        case Arrow:
            me.currentStyle = &me.qsArrow
        case Shape:
            me.currentStyle = &me.qsShape
        case Image:
            me.currentStyle = &me.qsImage
        case Step:
            me.currentStyle = &me.qsStep
        case Line:
            me.currentStyle = &me.qsLine
        case Polygon:
            me.currentStyle = &me.qsPolygon
        case Text:
            me.currentStyle = &me.qsText
        case Favorites:
            me.currentStyle = &me.qsFavorites
        case Draw:
            me.currentStyle = &me.qsDraw
        case Highlight:
            me.currentStyle = &me.qsHighlight
        case PixSel:
            me.currentStyle = &me.qsPixSel
        case Selection:
            return
        }

        foreach c in me.layout.childs:
            c.destroy()
        if !me.currentStyle:
            return

        foreach it in me.currentStyle.forms
        {
            let btn = Wnd.create'QuickButton(me.layout, null, {0, 0, 64, 64})
            btn.iconBtnFlags.add(.Checkable)
            btn.setForm(.RoundSquare)
            btn.formItf = it

            btn.sigPaintIcon = func(btn, bc, pos)
            {
                paintForm(bc.painter, cast(*QuickButton) btn)
            }

            btn.sigRightPressed += mtd||(btn)
            {
                var rect = btn.getSurfaceRect()
                me.btnPopup(btn, rect.center())
            }

            btn.sigCheckChanged += mtd|it|(btn)
            {
                if !btn.isChecked:
                    return
                me.main.updateModel(it)

                var capture = assume me.main.getCapture()
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

                me.updateSelection()
                me.main.updateState()
            }
        }

        let formItf     = me.main.editModeToModel(me.main.getEditMode())
        let baseForm    = cast(*BaseForm) @dataof(formItf)
        var canAddModel = false
        if baseForm
        {
            canAddModel = true
            if me.main.editMode == .Highlight
            {
                if baseForm.subKind != .Highlight:
                    canAddModel = false
            }
            elif me.main.editMode == .PixSel
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
            let btn = Wnd.create'QuickButton(me.layout, null, {0, 0, 64, 64})
            btn.userData = cast(*void) 1'u64 // This is the model button
            btn.formItf  = formItf
            btn.setForm(.RoundSquare)
            btn.style.addStyleSheetColors("btnIcon_Bk          0x0")
            btn.style.addStyleSheetColors("btnIcon_HotBk       0x0")
            btn.style.addStyleSheetColors("btnIcon_PressedBk   0x0")
            me.layout.sendResizeEvent()

            btn.sigPaintIcon = func(btn, bc, pos)
            {
                paintForm(bc.painter, cast(*QuickButton) btn)
                var rect = btn.getClientRect()
                rect.inflate(-2, -2)
                bc.painter.setParams(.Antialiased)
                bc.painter.drawRoundRect(rect, 5, 5, Pen.createDash(.Dash, Argb.White, 2))
            }

            // Button to add a new style
            let ico  = me.getTheme().getIcon24(.Plus)
            let btn1 = IconButton.create(btn, ico, {64 - 26, 2, 24, 24})
            btn1.style.addStyleSheetColors("btnIcon_Bk          $hilight")
            btn1.style.addStyleSheetColors("btnIcon_HotBk       $hilightLight")
            btn1.style.addStyleSheetColors("btnIcon_PressedBk   $hilightDark")
            btn1.setForm(.Round)

            me.layout.sendResizeEvent()
            me.setScrollSize(0, me.layout.resultHeight)

            btn1.sigPressed += mtd|me.main|(btn)
            {
                let mdl = main.newFormFromModel(main.getEditMode())
                if mdl
                {
                    me.currentStyle.forms.add(mdl)
                    me.updateQuickStyles()
                }
            }
        }

        me.updateSelection()
        me.layout.computeLayout()
    }
}
