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
        visit i: forms
            deleteOne(i)
        forms.clear()
    }
}

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

    qsArrow:        QuickStyle
    qsShape:        QuickStyle
    qsImage:        QuickStyle
    qsStep:         QuickStyle
    qsLine:         QuickStyle
    qsPolygon:      QuickStyle
    qsText:         QuickStyle
    qsFavorites:    QuickStyle
    qsDraw:         QuickStyle
    qsHighlight:    QuickStyle
    qsPixSel:       QuickStyle

    currentStyle:   *QuickStyle
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
                .serialize("QuickWnd.qsArrow",     &qsArrow)
                .serialize("QuickWnd.qsShape",     &qsShape)
                .serialize("QuickWnd.qsImage",     &qsImage)
                .serialize("QuickWnd.qsStep",      &qsStep)
                .serialize("QuickWnd.qsLine",      &qsLine)
                .serialize("QuickWnd.qsPolygon",   &qsPolygon)
                .serialize("QuickWnd.qsText",      &qsText)
                .serialize("QuickWnd.qsFavorites", &qsFavorites)
                .serialize("QuickWnd.qsDraw",      &qsDraw)
                .serialize("QuickWnd.qsHighlight", &qsHighlight)
                .serialize("QuickWnd.qsPixSel",    &qsPixSel)
            }
        }
    }
}

impl QuickWnd
{
    mtd setDefault()
    {
        if qsArrow.forms.isEmpty()      defaultQuickStyleArrow()
        if qsShape.forms.isEmpty()      defaultQuickStyleShape()
        if qsImage.forms.isEmpty()      defaultQuickStyleImage()
        if qsStep.forms.isEmpty()       defaultQuickStyleStep()
        if qsLine.forms.isEmpty()       defaultQuickStyleLine()
        if qsPolygon.forms.isEmpty()    defaultQuickStylePolygon()
        if qsText.forms.isEmpty()       defaultQuickStyleText()
        if qsDraw.forms.isEmpty()       defaultQuickStyleDraw()
        if qsHighlight.forms.isEmpty()  defaultQuickStyleHighlight()
        if qsPixSel.forms.isEmpty()     defaultQuickStylePixSel()
        updateQuickStyles()
    }

    mtd btnPopup(btn: *Button, pos: Math.Point)
    {
        menu := MenuCtrl.createPopup(self)
        menu.addItem("Remove Quick Style", "0", disabled = !btn)
        menu.addSeparator()
        menu.addItem(CmdIdResetQuickStyle)
        res := menu.doModal(pos, autoDestroy = true)
        if res == "0"
        {
            qbtn := cast(*QuickButton) btn
            currentStyle.remove(qbtn.formItf)
            qbtn.destroy()
            updateQuickStyles()
        }
    }

    func create(parent: *Wnd)->*QuickWnd
    {
        with quick := Wnd.create'QuickWnd(parent, {0, 0, 0, 300})
        {
            .backgroundStyle = .Window
            .scrollWndFlags |= .ForceVertical | .SmallBar | .TransparentBar

            .layout = WrapLayoutCtrl.create(quick)
            .layout.padding = 6
            .layout.spacing = 6
            .setView(quick.layout)
        }

        return quick
    }

    mtd defaultQuickStyleArrow()
    {
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
            #up qsArrow.forms.add(it)
        }

        const BorderSize = 25.0
        const ArrowSize  = 5.0

        add(0, BorderSize, ArrowSize, Argb.Red)
        add(0, BorderSize, ArrowSize, Argb.Green)
        add(0, BorderSize, ArrowSize, Argb.Blue)
        add(0, BorderSize, ArrowSize, Argb.Yellow)

        add(ArrowSize, BorderSize, ArrowSize, Argb.Red)
        add(ArrowSize, BorderSize, ArrowSize, Argb.Green)
        add(ArrowSize, BorderSize, ArrowSize, Argb.Blue)
        add(ArrowSize, BorderSize, ArrowSize, Argb.Yellow)
    }

    mtd defaultQuickStyleLine()
    {
        #[Swag.Macro]
        func add(borderSize: f32, color: Color)
        {
            form := Memory.new'FormLine()
            form.borderSize = borderSize
            form.borderColor = color
            it := cast(IForm) form
            it.setup(null)
            #up qsLine.forms.add(it)
        }

        const BorderSize = 25.0
        add(BorderSize, Argb.White)
        add(BorderSize, Argb.Gray)
        add(BorderSize, Argb.Black)
        add(BorderSize, Argb.Red)
        add(BorderSize, Argb.Green)
        add(BorderSize, Argb.Blue)
        add(BorderSize, Argb.Yellow)
        add(BorderSize, Argb.Purple)
    }

    mtd defaultQuickStyleShape()
    {
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
            #up qsShape.forms.add(it)
        }

        const BorderSize = 5.0
        add(BorderSize, .Ellipse, Argb.Red, Argb.Zero)
        add(BorderSize, .Ellipse, Argb.Purple, Argb.Zero)
        add(BorderSize, .Ellipse, Argb.Yellow, Argb.Zero)
        add(BorderSize, .RoundRectangle, Argb.Red, Argb.Zero)
        add(BorderSize, .RoundRectangle, Argb.Purple, Argb.Zero)
        add(BorderSize, .Rectangle, Argb.Red, Argb.Zero)
        add(BorderSize, .Rectangle, Argb.Purple, Argb.Zero)
        add(BorderSize, .Rectangle, Argb.White, Argb.Zero)
        add(BorderSize, .Rectangle, Argb.Black, Argb.Zero)
        add(0, .Rectangle, Argb.Zero, Argb.White)
        add(0, .Rectangle, Argb.Zero, Argb.Black)
    }

    mtd defaultQuickStyleImage()
    {
        #[Swag.Macro]
        func add(borderSize: f32, kind: FormImageKind, color: Color)
        {
            form := Memory.new'FormImage()
            form.borderSize = borderSize
            form.borderColor = color
            form.backColor = Argb.Gray
            form.kind = kind
            if borderSize == 0
                form.paintShadow = false
            it := cast(IForm) form
            it.setup(null)
            #up qsImage.forms.add(it)
        }

        add(2, .Image, Argb.White)
        add(2, .Zoom, Argb.White)
        add(0, .Blur, Argb.Zero)
        add(0, .Pixelate, Argb.Zero)
        add(0, .Capture, Argb.Zero)
    }

    mtd defaultQuickStyleStep()
    {
        #[Swag.Macro]
        func add(kind: FormStepKind, color: Color, backColor: Color)
        {
            form := Memory.new'FormStep()
            form.borderColor = color
            form.backColor = backColor
            form.kind = kind
            it := cast(IForm) form
            it.setup(null)
            #up qsStep.forms.add(it)
        }

        add(.Ellipse, Argb.White, Argb.Red)
        add(.Ellipse, Argb.White, Argb.Purple)
        add(.Ellipse, Argb.White, Argb.Green)
        add(.Ellipse, Argb.White, Argb.Orange)
        add(.RoundRectangle, Argb.White, Argb.Red)
        add(.RoundRectangle, Argb.White, Argb.Purple)
        add(.RoundRectangle, Argb.White, Argb.Green)
        add(.RoundRectangle, Argb.White, Argb.Orange)
    }

    mtd defaultQuickStylePolygon()
    {
        #[Swag.Macro]
        func add(borderSize: f32, color: Color)
        {
            form := Memory.new'FormPolygon()
            form.borderSize = borderSize
            form.borderColor = color
            form.backColor = 0
            it := cast(IForm) form
            it.setup(null)
            #up qsPolygon.forms.add(it)
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

    mtd defaultQuickStyleText()
    {
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
            #up qsText.forms.add(it)
        }

        add(3, 48, Argb.White, Argb.Red)
        add(3, 48, Argb.White, Argb.Green)
        add(3, 48, Argb.White, Argb.Blue)
        add(3, 48, Argb.Zero, Argb.White)
        add(3, 48, Argb.Zero, Argb.Black)

        add(4, 72, Argb.White, Argb.Red)
        add(4, 72, Argb.White, Argb.Green)
        add(4, 72, Argb.White, Argb.Blue)
        add(4, 72, Argb.Zero, Argb.White)
        add(4, 72, Argb.Zero, Argb.Black)
    }

    mtd defaultQuickStyleDraw()
    {
        #[Swag.Macro]
        func add(borderSize: f32, color: Color)
        {
            form := Memory.new'FormDraw()
            form.borderSize = borderSize
            form.borderColor = color
            it := cast(IForm) form
            it.setup(null)
            #up qsDraw.forms.add(it)
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

    mtd defaultQuickStyleHighlight()
    {
        #[Swag.Macro]
        func addDraw(borderSize: f32, color: Color)
        {
            form := Memory.new'FormDraw()
            form.borderSize = borderSize
            form.borderColor = color
            form.alpha = 30
            form.subKind = .Highlight
            it := cast(IForm) form
            it.setup(null)
            #up qsHighlight.forms.add(it)
        }

        #[Swag.Macro]
        func addShape(kind: FormShapeKind, backColor: Color)
        {
            form := Memory.new'FormShape()
            form.borderSize = 0
            form.borderColor = Argb.Zero
            form.backColor = backColor
            form.alpha = 30
            form.kind = kind
            form.subKind = .Highlight
            it := cast(IForm) form
            it.setup(null)
            #up qsHighlight.forms.add(it)
        }

        #[Swag.Macro]
        func addPoly(backColor: Color)
        {
            form := Memory.new'FormPolygon()
            form.borderSize = 0
            form.borderColor = Argb.Zero
            form.backColor = backColor
            form.alpha = 30
            form.subKind = .Highlight
            it := cast(IForm) form
            it.setup(null)
            #up qsHighlight.forms.add(it)
        }

        addShape(.Rectangle, Argb.Red)
        addShape(.Rectangle, Argb.Blue)
        addShape(.Rectangle, Argb.Green)
        addShape(.Rectangle, Argb.Yellow)

        const BorderSize = 20.0
        addDraw(BorderSize, Argb.Red)
        addDraw(BorderSize, Argb.Blue)
        addDraw(BorderSize, Argb.Green)
        addDraw(BorderSize, Argb.Yellow)

        addPoly(Argb.Red)
        addPoly(Argb.Blue)
        addPoly(Argb.Green)
        addPoly(Argb.Yellow)
    }

    mtd defaultQuickStylePixSel()
    {
        #[Swag.Macro]
        func addDraw()
        {
            form := Memory.new'FormDraw()
            form.subKind = .PixSel
            form.paintShadow = false
            it := cast(IForm) form
            it.setup(null)
            #up qsPixSel.forms.add(it)
        }

        #[Swag.Macro]
        func addShape(kind: FormShapeKind)
        {
            form := Memory.new'FormShape()
            form.kind = kind
            form.subKind = .PixSel
            form.paintShadow = false
            it := cast(IForm) form
            it.setup(null)
            #up qsPixSel.forms.add(it)
        }

        #[Swag.Macro]
        func addPoly()
        {
            form := Memory.new'FormPolygon()
            form.subKind = .PixSel
            form.paintShadow = false
            it := cast(IForm) form
            it.setup(null)
            #up qsPixSel.forms.add(it)
        }

        addShape(.Rectangle)
        addShape(.Ellipse)
        addPoly()
        addDraw()
    }

    mtd updateSelection()
    {
        if !currentStyle
            return
        done := false
        mode := main.getEditMode()

        firstVisible := 0's32
        visit c: layout.childs
        {
            if c.isVisible()
                break
            firstVisible += 1
        }

        visit it, i: currentStyle.forms
        {
            btn := cast(*IconButton) layout.childs[i + firstVisible]

            #[Swag.Complete]
            switch @kindof(it)
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

            if btn.isChecked
                done = true
        }

        curMdl := cast(*IconButton) layout.childs.back()
        if curMdl.userData
        {
            curMdl.show(!done)
            if !done
                curMdl.isChecked = true
        }

        invalidate()
    }

    mtd resetQuickStyles()
    {
        if !currentStyle
            return

        res := MessageDlg.confirm(surface, "This will reset the styles to the default values.")
        if res != MessageDlg.BtnYes
            return

        currentStyle.clear()

        #[Swag.Complete]
        switch main.editMode
        {
        case Arrow:     defaultQuickStyleArrow()
        case Shape:     defaultQuickStyleShape()
        case Image:     defaultQuickStyleImage()
        case Step:      defaultQuickStyleStep()
        case Line:      defaultQuickStyleLine()
        case Polygon:   defaultQuickStylePolygon()
        case Text:      defaultQuickStyleText()
        case Draw:      defaultQuickStyleDraw()
        case Highlight: defaultQuickStyleHighlight()
        case PixSel:    defaultQuickStylePixSel()
        case Favorites: break
        case Selection: break
        }

        updateQuickStyles()
    }

    mtd updateQuickStyles()
    {
        func paintForm(painter: *Painter, btnp: *QuickButton)
        {
            painter.setParams(.Antialiased)
            sel  := cast(*BaseForm) @dataof(btnp.formItf)
            s    := cast(s32) BaseForm.adaptSize(sel.borderSize / 2, MinBorderSize, MaxBorderSize, 1, true)
            rect := Math.Rectangle{8 + s, 8 + s, 48 - 2*s, 48 - 2*s}

            title := ""

            switch @kindof(btnp.formItf)
            {
            case FormPolygon:
                poly := cast(*FormPolygon) @dataof(btnp.formItf)
                poly.setDefaultShape()
            case FormDraw:
                poly := cast(*FormDraw) @dataof(btnp.formItf)
                poly.setDefaultShape()
            case FormText:
                txt := cast(*FormText) @dataof(btnp.formItf)
                txt.text = "A"
            case FormImage:
                img := cast(*FormImage) @dataof(btnp.formItf)
                switch img.kind
                {
                case Image:     title = "image"
                case Blur:      title = "blur"
                case Pixelate:  title = "pixelate"
                case Zoom:      title = "zoom"
                case Capture:   title = "capture"
                }
            }

            btnp.formItf.setBaseForm(acast rect.x, acast rect.y, acast rect.width, acast rect.height)
            Capture.paintForm(btnp.getApp(), painter, {0, 0, 64, 64}, btnp.formItf, zoom = 1, quickMode = true)

            if @countof(title)
                painter.drawStringCenter(rect.horzCenter(), rect.bottom(), title, btnp.getFont().regular, Argb.White)
        }

        edMode := main.editMode == .Selection ? main.getEditMode() : main.editMode
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

        visit c: layout.childs
            c.destroy()
        if !currentStyle
            return

        visit it: currentStyle.forms
        {
            btn := Wnd.create'QuickButton(layout, null, {0, 0, 64, 64})
            btn.iconBtnFlags.add(.Checkable)
            btn.setForm(.RoundSquare)
            btn.formItf = it

            btn.sigPaintIcon = func(btn, bc, pos) {
                paintForm(bc.painter, cast(*QuickButton) btn)
            }

            btn.sigRightPressed += closure|self|(btn) {
                rect := btn.getSurfaceRect()
                self.btnPopup(btn, rect.center())
            }

            btn.sigCheckChanged += closure|self, it|(btn) {
                if !btn.isChecked
                    return

                type := @kindof(it)

                #[Swag.Complete]
                switch type
                {
                case FormShape:
                    b := cast(*FormShape) @dataof(it)
                    self.main.mdlFormShape = dref b
                    self.main.mdlFavorite  = &self.main.mdlFormShape
                    self.main.mdlHighlight = &self.main.mdlFormShape
                    self.main.mdlPixSel    = &self.main.mdlFormShape
                case FormImage:
                    b := cast(*FormImage) @dataof(it)
                    self.main.mdlFormImage = dref b
                    self.main.mdlFavorite  = &self.main.mdlFormImage
                case FormStep:
                    b := cast(*FormStep) @dataof(it)
                    self.main.mdlFormStep  = dref b
                    self.main.mdlFavorite  = &self.main.mdlFormStep
                case FormLine:
                    b := cast(*FormLine) @dataof(it)
                    self.main.mdlFormLine  = dref b
                    self.main.mdlFavorite  = &self.main.mdlFormLine
                    self.main.mdlHighlight = &self.main.mdlFormLine
                case FormPolygon:
                    b := cast(*FormPolygon) @dataof(it)
                    self.main.mdlFormPolygon = dref b
                    self.main.mdlFavorite  = &self.main.mdlFormPolygon
                    self.main.mdlHighlight = &self.main.mdlFormPolygon
                    self.main.mdlPixSel    = &self.main.mdlFormPolygon
                case FormArrow:
                    b := cast(*FormArrow) @dataof(it)
                    self.main.mdlFormArrow = dref b
                    self.main.mdlFavorite = &self.main.mdlFormArrow
                case FormText:
                    b := cast(*FormText) @dataof(it)
                    self.main.mdlFormText = dref b
                    self.main.mdlFavorite = &self.main.mdlFormText
                case FormDraw:
                    b := cast(*FormDraw) @dataof(it)
                    self.main.mdlFormDraw  = dref b
                    self.main.mdlFavorite  = &self.main.mdlFormDraw
                    self.main.mdlHighlight = &self.main.mdlFormDraw
                    self.main.mdlPixSel    = &self.main.mdlFormDraw
                }

                capture := self.main.getCapture()
                if @kindof(it) == type and capture.selection.count
                {
                    var mdlValue: *BaseForm

                    #[Swag.Complete]
                    switch type
                    {
                    case FormShape:
                        b := cast(*FormShape) @dataof(it)
                        mdlValue = Memory.new'FormShape()
                        it.copy(mdlValue, b)
                    case FormImage:
                        b := cast(*FormImage) @dataof(it)
                        mdlValue = Memory.new'FormImage()
                        it.copy(mdlValue, b)
                    case FormStep:
                        b := cast(*FormStep) @dataof(it)
                        mdlValue = Memory.new'FormStep()
                        it.copy(mdlValue, b)
                    case FormLine:
                        b := cast(*FormLine) @dataof(it)
                        mdlValue = Memory.new'FormLine()
                        it.copy(mdlValue, b)
                    case FormPolygon:
                        b := cast(*FormPolygon) @dataof(it)
                        mdlValue = Memory.new'FormPolygon()
                        it.copy(mdlValue, b)
                    case FormArrow:
                        b := cast(*FormArrow) @dataof(it)
                        mdlValue = Memory.new'FormArrow()
                        it.copy(mdlValue, b)
                    case FormText:
                        b := cast(*FormText) @dataof(it)
                        mdlValue = Memory.new'FormText()
                        it.copy(mdlValue, b)
                    case FormDraw:
                        b := cast(*FormDraw) @dataof(it)
                        mdlValue = Memory.new'FormDraw()
                        it.copy(mdlValue, b)
                    }

                    undo := capture.newUndo(.SetFormFromModel)
                    undo.mdlType = type
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

        // The button with the current model
        mode := main.getEditMode()

        var formItf: IForm
        #[Swag.Complete]
        switch mode
        {
        case Arrow:
            formItf = cast(IForm) main.mdlFormArrow
        case Shape:
            formItf = cast(IForm) main.mdlFormShape
        case Image:
            formItf = cast(IForm) main.mdlFormImage
        case Step:
            formItf = cast(IForm) main.mdlFormStep
        case Line:
            formItf = cast(IForm) main.mdlFormLine
        case Polygon:
            formItf = cast(IForm) main.mdlFormPolygon
        case Text:
            formItf = cast(IForm) main.mdlFormText
        case Draw:
            formItf = cast(IForm) main.mdlFormDraw
        case Favorites:
            formItf = main.mdlFavorite
        case Highlight:
            formItf = main.mdlHighlight
        case PixSel:
            formItf = main.mdlPixSel
        case Selection:
            break
        }

        baseForm := cast(*BaseForm) @dataof(formItf)
        canAddModel := true
        if main.editMode == .Highlight
        {
            if baseForm.subKind != .Highlight
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

        if canAddModel
        {
            btn := Wnd.create'QuickButton(layout, null, {0, 0, 64, 64})
            btn.userData = cast(*void) 1'u64 // This is the model button
            btn.formItf = formItf
            btn.setForm(.RoundSquare)
            btn.style.addStyleSheetColors("btnIcon_Bk          0x0")
            btn.style.addStyleSheetColors("btnIcon_HotBk       0x0")
            btn.style.addStyleSheetColors("btnIcon_PressedBk   0x0")
            layout.sendResizeEvent()

            btn.sigPaintIcon = func(btn, bc, pos) {
                paintForm(bc.painter, cast(*QuickButton) btn)
                rect := btn.getClientRect()
                rect.inflate(-2, -2)
                bc.painter.setParams(.Antialiased)
                bc.painter.drawRoundRect(rect, 5, 5, Pen.createDash(.Dash, Argb.White, 2))
            }

            // Button to add a new style
            ico  := getTheme().getIcon24(ThemeIcons24.Plus)
            btn1 := IconButton.create(btn, ico, {64 - 26, 2, 24, 24})
            btn1.style.addStyleSheetColors("btnIcon_Bk          $hilight")
            btn1.style.addStyleSheetColors("btnIcon_HotBk       $hilightLight")
            btn1.style.addStyleSheetColors("btnIcon_PressedBk   $hilightDark")
            btn1.setForm(.Round)

            layout.sendResizeEvent()
            setScrollSize(0, layout.resultHeight)

            btn1.sigPressed += closure|self, main|(btn) {
                mode := main.getEditMode()
                #[Swag.Complete]
                switch mode
                {
                case Arrow:
                    mdl := Memory.new'FormArrow()
                    mdl.IForm.copy(mdl, &main.mdlFormArrow)
                    self.currentStyle.forms.add(mdl)
                case Shape:
                    mdl := Memory.new'FormShape()
                    mdl.IForm.copy(mdl, &main.mdlFormShape)
                    self.currentStyle.forms.add(mdl)
                case Image:
                    mdl := Memory.new'FormImage()
                    mdl.IForm.copy(mdl, &main.mdlFormImage)
                    self.currentStyle.forms.add(mdl)
                case Step:
                    mdl := Memory.new'FormStep()
                    mdl.IForm.copy(mdl, &main.mdlFormStep)
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
                case Draw:
                    mdl := Memory.new'FormDraw()
                    mdl.IForm.copy(mdl, &main.mdlFormDraw)
                    self.currentStyle.forms.add(mdl)
                case Favorites, Highlight, Selection, PixSel:
                    break
                }

                self.updateQuickStyles()
            }
        }

        updateSelection()
        layout.computeLayout()
    }
}