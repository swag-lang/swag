const MinBorderSize     = 0.0
const MaxBorderSize     = 50.0
const MinTextBorderSize = 0.0
const MaxTextBorderSize = 10.0
const MinCapSize        = 1.0
const MaxCapSize        = 8.0
const MinFontSize       = 10.0
const MaxFontSize       = 150.0
const MinBoldSize       = 0.0
const MaxBoldSize       = 10.0

const CapSize         = 8.0
const DashSize        = 8.0
const BtnColorWidth   = 68.0
const BtnColorHeight  = 88.0
const BtnStyleWidth   = 88.0
const BtnStyleHeight  = 68.0

const CmdIdTextHLeft    = "CmdIdTextHLeft"
const CmdIdTextHCenter  = "CmdIdTextHCenter"
const CmdIdTextHRight   = "CmdIdTextHRight"
const CmdIdTextVTop     = "CmdIdTextVTop"
const CmdIdTextVCenter  = "CmdIdTextVCenter"
const CmdIdTextVBottom  = "CmdIdTextVBottom"
const CmdIdTextBold     = "CmdIdTextBold"
const CmdIdTextItalic   = "CmdIdTextItalic"

struct PropWnd
{
    using scrollWnd:    ScrollWnd
    mainLayout:         *StackLayoutCtrl
    main:               *MainWnd
    sysFonts:           Array'FontFamilyEnumerate
}

impl PropWnd
{
    mtd getSelectedBaseForm()->*BaseForm
    {
        capture := main.getCapture()
        if !capture or capture.selection.count == 0
        {
            #[Swag.Complete]
            switch main.editMode
            {
            case Arrow:
                return cast(*BaseForm) &main.mdlFormArrow
            case Line:
                return cast(*BaseForm) &main.mdlFormLine
            case Shape:
                return cast(*BaseForm) &main.mdlFormShape
            case Image:
                return cast(*BaseForm) &main.mdlFormImage
            case Step:
                return cast(*BaseForm) &main.mdlFormStep
            case Polygon:
                return cast(*BaseForm) &main.mdlFormPolygon
            case Text:
                return cast(*BaseForm) &main.mdlFormText
            case Draw:
                return cast(*BaseForm) &main.mdlFormDraw
            case Highlight:
                return @dataof(main.mdlHighlight)
            case PixSel:
                return @dataof(main.mdlPixSel)
            case Favorites, Selection:
                return @dataof(main.mdlFavorite)
            }
        }

        return capture.getSelectedBaseForm()
    }

    mtd getSelectedFormType()->typeinfo
    {
        capture := main.getCapture()
        if !capture or capture.selection.count == 0
        {
            #[Swag.Complete]
            switch main.editMode
            {
            case Arrow:
                return FormArrow
            case Line:
                return FormLine
            case Shape:
                return FormShape
            case Image:
                return FormImage
            case Step:
                return FormStep
            case Polygon:
                return FormPolygon
            case Text:
                return FormText
            case Draw:
                return FormDraw
            case Highlight:
                return @kindof(main.mdlHighlight)
            case PixSel:
                return @kindof(main.mdlPixSel)
            case Favorites, Selection:
                return @kindof(main.mdlFavorite)
            }
        }

        return capture.getSelectedFormType()
    }

    mtd pushTypedValue(val: u32, type: typeinfo, name: string, first: bool)
    {
        capture := main.getCapture()

        // Change model
        if !capture or !capture.selection.count
        {
            form   := getSelectedBaseForm()
            field  := Reflection.getField(#self.type, #self.name)
            valU32 := val
            Reflection.setFieldValue(form, field, &valU32, true)
        }

        // Change selection
        else
        {
            if !first
                capture.undo()
            undo := capture.newUndo(.SetTypedValue)
            undo.valU32 = val
            undo.valRef = Reflection.getField(#self.type, #self.name)
            capture.pushUndo(undo)
        }

        main.quickWnd.updateSelection()
        main.editWnd.invalidate()
    }

    mtd createSlider(name: string, minv, maxv: s64, val: f32)->*SliderCtrl
    {
        with ctrl := SliderCtrl.create(mainLayout, #self.name)
        {
            .setMinMaxS64(minv, maxv)
            .dockStyle = .Top
            .setValue(val)
        }

        return ctrl
    }

    func createArrowButton(stack: *Wnd, w, h: f32)->*IconButton
    {
        with btn := IconButton.create(stack, {}, {0, 0, w, h})
        {
            .setForm(.RoundSquare)
            .style.addStyleSheetColors("btnIcon_Bk        $lighterMedium")
            .style.addStyleSheetColors("btnIcon_HotBk     $lighterBig")
            .style.addStyleSheetColors("btnIcon_PressedBk $lighterVeryBig")
            .iconBtnFlags.add(.Popup | .CheckableToggle)
            .arrowPos = .Bottom
            .textPos = .Top
            .checkedForm = .None
        }

        return btn
    }

    func createPopup(btn: *IconButton, w, h: f32)->*FrameWnd
    {
        pos     := btn.getSurfaceRect()
        topView := btn.getTopView()
        pt      := topView.surfaceToLocal({pos.x, pos.bottom()})
        pt.y += btn.getThemeMetrics().frameWnd_AnchorHeight

        sdw := btn.getThemeMetrics().frameWnd_SmallShadowSize

        with frame := Wnd.create'FrameWnd(topView, {pt.x, pt.y, w, h})
        {
            .setFrameForm(.Round)
            .setFrameFlags(FrameWndFlags.SmallShadow)
            .style.addStyleSheetColors("frameWnd_Border $frameWnd_PopupBorder")
            .fitPosInParent(sdw.z, sdw.w)
            .anchor = .Top
            .anchorPos = pos.horzCenter() - frame.getSurfaceRect().x
        }

        btn.popup = frame
        return frame
    }

    func createColorPicker(btn: *IconButton, color: Color)->*ColorPickerCtrl
    {
        frame := createPopup(btn, 356, 382)

        picker := ColorPickerCtrl.create(frame, {8, 54})
        picker.setColor(color)

        with palette := PalettePicker.create(frame, {16, 16, 340, 48})
        {
            .addColor(Argb.White)
            .addColor(Argb.Gray)
            .addColor(Argb.Black)
            .addColor(Argb.Red)
            .addColor(Argb.Green)
            .addColor(Argb.Blue)
            .addColor(Argb.Yellow)
            .addColor(Argb.Purple)
            .addColor(Color.fromArgb(0))
            .boxSize = 32
            .sigChanged += closure|picker|(pal) {
                picker.setColor(pal.selectedColor)
            }
        }

        return picker
    }

    mtd createPopupShapeKind(btn: *IconButton)
    {
        frame := createPopup(btn, 180, 66)
        layout := frame.createView'WrapLayoutCtrl()
        layout.wrapWidth, layout.wrapHeight = 48
        layout.padding = 4

        mtd updateBtnState(layout: *Wnd)
        {
            curSel := cast(*FormShape) getSelectedBaseForm()
            visit c: layout.childs
            {
                btn := cast(*IconButton) c
                kind := cast(FormShapeKind) cast(u64) btn.userData
                btn.isChecked = curSel.kind == kind
                btn.invalidate()
            }
        }

        curSel := cast(*FormShape) getSelectedBaseForm()

        loop i: @countof(FormShapeKind)
        {
            btn0 := IconButton.create(frame.view, {})
            btn0.userData = cast(*void) cast(u64) i
            btn0.setForm(.RoundSquare)
            if curSel.kind == i btn0.isChecked = true

            me := self
            btn0.sigPaintIcon = closure|me|(btn, bc, pos)
            {
                bc.painter.setParams(.Antialiased)
                curSel := cast(*FormShape) me.getSelectedBaseForm()
                rect := pos
                rect.inflate(-9, -9)
                rect.x = rect.horzCenter() - rect.height * 0.5
                rect.width = rect.height
                color := btn.getThemeColors().btnIcon_Icon

                #[Swag.Complete]
                switch cast(FormShapeKind) cast(u64) btn.userData
                {
                case Ellipse:
                    radius := rect.width * 0.5
                    bc.painter.drawEllipse(pos.horzCenter(), pos.vertCenter(), radius, radius, color, 5)
                case Rectangle:
                    bc.painter.drawRect(rect, color, 5)
                case RoundRectangle:
                    bc.painter.drawRoundRect(rect, 5, 5, color, 5)
                }
            }

            btn0.sigPressed += closure|me, layout|(btn) {
                kind := cast(u64) btn.userData
                me.pushTypedValue(acast kind, @typeof(FormShape), @nameof(FormShape.kind), true)
                me.updateBtnState(layout)
            }
        }

        layout.computeLayout()
    }

    mtd createPopupStepKind(btn: *IconButton)
    {
        frame := createPopup(btn, 180, 66)
        layout := frame.createView'WrapLayoutCtrl()
        layout.wrapWidth, layout.wrapHeight = 48
        layout.padding = 4

        loop i: @countof(FormStepKind)
        {
            btn0 := IconButton.create(frame.view, {})
            btn0.userData = cast(*void) cast(u64) i
            btn0.setForm(.RoundSquare)
            me := self
            btn0.sigPaintIcon = closure|me|(btn, bc, pos)
            {
                bc.painter.setParams(.Antialiased)
                curSel := cast(*FormStep) me.getSelectedBaseForm()
                rect := pos
                rect.inflate(-9, -9)
                rect.x = rect.horzCenter() - rect.height * 0.5
                rect.width = rect.height
                color := btn.getThemeColors().btnIcon_Icon

                #[Swag.Complete]
                switch cast(FormStepKind) cast(u64) btn.userData
                {
                case Ellipse:
                    radius := rect.width * 0.5
                    bc.painter.drawEllipse(pos.horzCenter(), pos.vertCenter(), radius, radius, color, 5)
                case Rectangle:
                    bc.painter.drawRect(rect, color, 5)
                case RoundRectangle:
                    bc.painter.drawRoundRect(rect, 5, 5, color, 5)
                }
            }

            btn0.sigPressed += closure|me|(btn) {
                kind := cast(u64) btn.userData
                me.pushTypedValue(acast kind, @typeof(FormStep), @nameof(FormStep.kind), true)
            }
        }

        layout.computeLayout()
    }


    mtd createPopupStartCapStyle(btn: *IconButton)
    {
        frame := createPopup(btn, 100, 280)
        layout := StackLayoutCtrl.create(frame, .Top)
        frame.setView(layout)

        mtd updateBtnState(layout: *Wnd)
        {
            curSel := cast(*FormLine) getSelectedBaseForm()
            visit c: layout.childs
            {
                btn := cast(*IconButton) c
                cap := cast(LineCapStyle) cast(u64) btn.userData
                btn.isChecked = curSel.beginCap == cap
                btn.invalidate()
            }
        }

        curSel := cast(*FormLine) getSelectedBaseForm()
        visit i: LineCapStyle
        {
            btn0 := IconButton.create(layout, {}, {0, 0, 0, 40})
            btn0.userData = cast(*void) cast(u64) i
            btn0.setForm(.RoundSquare)
            btn0.isChecked = curSel.beginCap == i

            me := self
            btn0.sigPaintIcon = closure|me|(btn, bc, pos)
            {
                bc.painter.setParams(.Antialiased)
                curSel := cast(*FormShape) me.getSelectedBaseForm()
                rect := pos
                rect.inflate(-9, -9)
                cap := cast(LineCapStyle) cast(u64) btn.userData
                pen := Pen.createSolid(btn.getThemeColors().btnIcon_Icon, CapSize)
                pen.beginCapStyle = cap
                pen.endCapStyle = .None
                rect.moveLeft(FormLine.drawCapOffset(cap, CapSize, pen.capStartScaleX))
                bc.painter.drawLine(rect.x, rect.vertCenter(), rect.right(), rect.vertCenter(), pen)
            }

            btn0.sigPressed += closure|me, layout|(btn) {
                kind := cast(u64) btn.userData
                me.pushTypedValue(acast kind, @typeof(FormLine), @nameof(FormLine.beginCap), true)
                me.updateBtnState(layout)
            }
        }

        layout.computeLayout()
    }

    mtd createPopupEndCapStyle(btn: *IconButton)
    {
        frame := createPopup(btn, 100, 280)
        layout := StackLayoutCtrl.create(frame, .Top)
        frame.setView(layout)

        mtd updateBtnState(layout: *Wnd)
        {
            curSel := cast(*FormLine) getSelectedBaseForm()
            visit c: layout.childs
            {
                btn := cast(*IconButton) c
                cap := cast(LineCapStyle) cast(u64) btn.userData
                btn.isChecked = curSel.endCap == cap
                btn.invalidate()
            }
        }

        curSel := cast(*FormLine) getSelectedBaseForm()
        visit i: LineCapStyle
        {
            btn0 := IconButton.create(layout, {}, {0, 0, 0, 40})
            btn0.userData = cast(*void) cast(u64) i
            btn0.setForm(.RoundSquare)
            btn0.isChecked = curSel.endCap == i

            me := self
            btn0.sigPaintIcon = closure|me|(btn, bc, pos)
            {
                bc.painter.setParams(.Antialiased)
                curSel := cast(*FormShape) me.getSelectedBaseForm()
                rect := pos
                rect.inflate(-9, -9)
                cap := cast(LineCapStyle) cast(u64) btn.userData
                pen := Pen.createSolid(btn.getThemeColors().btnIcon_Icon, CapSize)
                pen.beginCapStyle = .None
                pen.endCapStyle = cap
                rect.moveRight(-FormLine.drawCapOffset(cap, CapSize, pen.capStartScaleX))
                bc.painter.drawLine(rect.x, rect.vertCenter(), rect.right(), rect.vertCenter(), pen)
            }

            btn0.sigPressed += closure|me, layout|(btn) {
                kind := cast(u64) btn.userData
                me.pushTypedValue(acast kind, @typeof(FormLine), @nameof(FormLine.endCap), true)
                me.updateBtnState(layout)
            }
        }

        layout.computeLayout()
    }

    mtd createPopupDashStyle(btn: *IconButton)
    {
        frame := createPopup(btn, 180, 170)
        layout := StackLayoutCtrl.create(frame, .Top)
        frame.setView(layout)

        mtd updateBtnState(layout: *Wnd)
        {
            curSel := getSelectedBaseForm()
            visit c: layout.childs
            {
                btn := cast(*IconButton) c
                dash := cast(DashStyle) cast(u64) btn.userData
                btn.isChecked = curSel.dashStyle == dash
                btn.invalidate()
            }
        }

        curSel := getSelectedBaseForm()
        visit i: DashStyle
        {
            if i == .Pattern continue

            btn0 := IconButton.create(layout, {}, {0, 0, 0, 32})
            btn0.userData = cast(*void) cast(u64) i
            btn0.setForm(.RoundSquare)
            btn0.isChecked = curSel.dashStyle == i

            me := self
            btn0.sigPaintIcon = closure|me|(btn, bc, pos)
            {
                bc.painter.setParams(.Antialiased)
                curSel := cast(*FormShape) me.getSelectedBaseForm()
                rect := pos
                rect.inflate(-9, -9)
                dash := cast(DashStyle) cast(u64) btn.userData
                pen := Pen.createDash(dash, btn.getThemeColors().btnIcon_Icon, DashSize)
                bc.painter.drawLine(rect.x, rect.vertCenter(), rect.right(), rect.vertCenter(), pen)
            }

            btn0.sigPressed += closure|me, layout|(btn) {
                kind := cast(u64) btn.userData
                me.pushTypedValue(acast kind, @typeof(BaseForm), @nameof(BaseForm.dashStyle), true)
                me.updateBtnState(layout)
            }
        }

        layout.computeLayout()
    }

    mtd propBorderColor(stack: *Wnd)
    {
        curSel := cast(*FormText) getSelectedBaseForm()
        if curSel.subKind == .Highlight or curSel.subKind == .PixSel
            return

        btn := createArrowButton(stack, BtnColorWidth, BtnColorHeight)

        curSelType := getSelectedFormType()
        switch curSelType
        {
        case FormLine, FormArrow, FormDraw:
            btn.name = "Color"
        case FormStep:
            btn.name = "Text"
        default:
            btn.name = "Outline"
        }

        me := self
        btn.sigPaintIcon = closure|me|(btn, bc, pos)
        {
            bc.painter.setParams(.Antialiased)
            curSel := me.getSelectedBaseForm()
            rect := pos
            rect.x = rect.horzCenter() - 16
            rect.y = rect.vertCenter() - 16
            rect.width, rect.height = 32
            if !curSel.borderColor.isOpaque()
                bc.painter.drawRoundRect(rect, 5, 5, Pen.createHatch(.CheckBoardLarge, Argb.DarkGray, 5))
            bc.painter.drawRoundRect(rect, 5, 5, curSel.borderColor, 5)
        }

        btn.sigShowPopup += closure|me|(btn) {
            curSel := me.getSelectedBaseForm()
            picker := createColorPicker(btn, curSel.borderColor)
            me.pushTypedValue(curSel.borderColor.argb, @typeof(BaseForm), @nameof(BaseForm.borderColor), true)
            picker.sigChanged += closure|me, btn|(pk) {
                me.pushTypedValue(pk.getColor().argb, @typeof(BaseForm), @nameof(BaseForm.borderColor), false)
                btn.invalidate()
            }
        }
    }

    mtd propBackColor(stack: *Wnd)
    {
        curSel := getSelectedBaseForm()
        if curSel.subKind != .PixSel
        {
            curSelType := getSelectedFormType()
            if curSelType == FormLine or
            curSelType == FormArrow or
            curSelType == FormDraw
                return

            if curSelType == FormImage
            {
                imgSel := cast(*FormImage) curSel
                if imgSel.kind != .Image
                    return
            }
        }

        btn := createArrowButton(stack, BtnColorWidth, BtnColorHeight)
        btn.name = "Fill"
        me := self
        btn.sigPaintIcon = closure|me|(btn, bc, pos)
        {
            bc.painter.setParams(.Antialiased)
            curSel := me.getSelectedBaseForm()
            rect := pos
            rect.x = rect.horzCenter() - 21
            rect.y = rect.vertCenter() - 21
            rect.width, rect.height = 42
            backCol := curSel.subKind == .PixSel ? g_Main.pixSelFillCol : curSel.backColor
            if !backCol.isOpaque()
                bc.painter.fillRoundRect(rect, 10, 10, Brush.createHatch(.CheckBoardLarge, Argb.DarkGray))
            bc.painter.fillRoundRect(rect, 10, 10, backCol)
        }

        btn.sigShowPopup += closure|me|(btn) {
            curSel := me.getSelectedBaseForm()
            backCol := curSel.subKind == .PixSel ? g_Main.pixSelFillCol : curSel.backColor
            picker := createColorPicker(btn, backCol)
            if curSel.subKind == .PixSel
            {
                picker.sigChanged += closure|btn|(pk) {
                    g_Main.pixSelFillCol = pk.getColor()
                    btn.invalidate()
                }
            }
            else
            {
                me.pushTypedValue(curSel.backColor.argb, @typeof(BaseForm), @nameof(BaseForm.backColor), true)
                picker.sigChanged += closure|me, btn|(pk) {
                    me.pushTypedValue(pk.getColor().argb, @typeof(BaseForm), @nameof(BaseForm.backColor), false)
                    btn.invalidate()
                }
            }
        }
    }

    mtd propShape(stack: *Wnd)
    {
        curSelType := getSelectedFormType()
        if curSelType != FormShape
            return
        curSel := getSelectedBaseForm()
        if curSel.subKind == .PixSel
            return

        btn := createArrowButton(stack, BtnColorWidth, BtnColorHeight)
        btn.name = "Shape"
        me := self
        btn.sigPaintIcon = closure|me|(btn, bc, pos)
        {
            bc.painter.setParams(.Antialiased)
            curSel := cast(*FormShape) me.getSelectedBaseForm()
            rect := pos
            rect.inflate(-9, -9)
            rect.x = rect.horzCenter() - rect.height * 0.5
            rect.width = rect.height
            color := btn.getThemeColors().btnIcon_Icon

            #[Swag.Complete]
            switch curSel.kind
            {
            case Ellipse:
                radius := 16.0
                bc.painter.drawEllipse(pos.horzCenter(), pos.vertCenter(), radius, radius, color, 5)
            case Rectangle:
                bc.painter.drawRect(rect, color, 5)
            case RoundRectangle:
                bc.painter.drawRoundRect(rect, 5, 5, color, 5)
            }
        }

        btn.sigShowPopup += closure|me|(btn) {
            curSel := cast(*FormShape) me.getSelectedBaseForm()
            me.createPopupShapeKind(btn)
        }
    }

    mtd propStep(stack: *Wnd)
    {
        curSelType := getSelectedFormType()
        if curSelType != FormStep
            return

        btn := createArrowButton(stack, BtnColorWidth, BtnColorHeight)
        btn.name = "Shape"
        me := self
        btn.sigPaintIcon = closure|me|(btn, bc, pos)
        {
            bc.painter.setParams(.Antialiased)
            curSel := cast(*FormStep) me.getSelectedBaseForm()
            rect := pos
            rect.inflate(-9, -9)
            rect.x = rect.horzCenter() - rect.height * 0.5
            rect.width = rect.height
            color := btn.getThemeColors().btnIcon_Icon

            #[Swag.Complete]
            switch curSel.kind
            {
            case Ellipse:
                radius := 16.0
                bc.painter.drawEllipse(pos.horzCenter(), pos.vertCenter(), radius, radius, color, 5)
            case Rectangle:
                bc.painter.drawRect(rect, color, 5)
            case RoundRectangle:
                bc.painter.drawRoundRect(rect, 5, 5, color, 5)
            }
        }

        btn.sigShowPopup += closure|me|(btn) {
            curSel := cast(*FormStep) me.getSelectedBaseForm()
            me.createPopupStepKind(btn)
        }
    }

    mtd propStartCapStyle(stack: *Wnd)
    {
        curSelType := getSelectedFormType()
        if curSelType != FormLine and curSelType != FormArrow
            return

        btn := createArrowButton(stack, BtnStyleWidth, BtnStyleHeight)
        btn.name = "Start"
        me := self
        btn.sigPaintIcon = closure|me|(btn, bc, pos)
        {
            bc.painter.setParams(.Antialiased)
            curSel := cast(*FormLine) me.getSelectedBaseForm()
            rect := pos
            rect.inflate(-9, -9)
            pen := Pen.createSolid(btn.getThemeColors().btnIcon_Icon, CapSize)
            pen.beginCapStyle = curSel.beginCap
            pen.endCapStyle = .None
            rect.moveLeft(FormLine.drawCapOffset(curSel.beginCap, CapSize, pen.capStartScaleX))
            bc.painter.drawLine(rect.x, rect.vertCenter(), rect.right(), rect.vertCenter(), pen)
        }

        btn.sigShowPopup += closure|me|(btn) {
            curSel := cast(*FormShape) me.getSelectedBaseForm()
            me.createPopupStartCapStyle(btn)
        }
    }

    mtd propEndCapStyle(stack: *Wnd)
    {
        curSelType := getSelectedFormType()
        if curSelType != FormLine and curSelType != FormArrow
            return

        btn := createArrowButton(stack, BtnStyleWidth, BtnStyleHeight)
        btn.name = "End"
        me := self
        btn.sigPaintIcon = closure|me|(btn, bc, pos)
        {
            bc.painter.setParams(.Antialiased)
            curSel := cast(*FormLine) me.getSelectedBaseForm()
            rect := pos
            rect.inflate(-9, -9)
            pen := Pen.createSolid(btn.getThemeColors().btnIcon_Icon, CapSize)
            pen.beginCapStyle = .None
            pen.endCapStyle = curSel.endCap
            rect.moveRight(-FormLine.drawCapOffset(curSel.endCap, CapSize, pen.capStartScaleX))
            bc.painter.drawLine(rect.x, rect.vertCenter(), rect.right(), rect.vertCenter(), pen)
        }

        btn.sigShowPopup += closure|me|(btn) {
            curSel := cast(*FormShape) me.getSelectedBaseForm()
            me.createPopupEndCapStyle(btn)
        }
    }

    mtd propDashStyle(stack: *Wnd)
    {
        curSelType := getSelectedFormType()
        if curSelType != FormShape and curSelType != FormLine and curSelType != FormArrow and curSelType != FormPolygon
            return

        curSel := cast(*FormText) getSelectedBaseForm()
        if curSel.subKind == .Highlight or curSel.subKind == .PixSel
            return

        btn := createArrowButton(stack, BtnStyleWidth, BtnStyleHeight)
        btn.name = "Dash"
        me := self
        btn.sigPaintIcon = closure|me|(btn, bc, pos)
        {
            bc.painter.setParams(.Antialiased)
            curSel := me.getSelectedBaseForm()
            rect := pos
            rect.inflate(-9, -9)
            pen := Pen.createDash(curSel.dashStyle, btn.getThemeColors().btnIcon_Icon, DashSize)
            bc.painter.drawLine(rect.x, rect.vertCenter(), rect.right(), rect.vertCenter(), pen)
        }

        btn.sigShowPopup += closure|me|(btn) {
            curSel := cast(*FormShape) me.getSelectedBaseForm()
            me.createPopupDashStyle(btn)
        }
    }

    mtd propNextStep()
    {
        curSelType := getSelectedFormType()
        if curSelType != FormStep
            return

        stack := Wnd.create'Wnd(mainLayout, {0, 0, 0, getThemeMetrics().comboBox_Height})

        curSel := cast(*FormStep) getSelectedBaseForm()
        with combo := ComboBox.create(stack)
        {
            combo.dockStyle = .Center
            .addItem("1...2...3...")
            .addItem("A...B...C...")
            .addItem("a...b...c...")
            .selectItem(cast(u32) curSel.typeIndex)
            .sigChanged += closure|self|(combo, val) {
                self.pushTypedValue(acast val, @typeof(FormStep), @nameof(FormStep.typeIndex), true)
            }
        }

        with btn := PushButton.create(stack, "Reset", {0, 0, 60})
        {
            .margin.x = 5
            .dockStyle = .Right
            .toolTip = "Reset the global step count to 1"
            btn.sigPressed += closure|self|(btn) {
                self.main.getCapture().nextStepIndex = 1
            }
        }
    }

    mtd editText(curText: String)->String
    {
        with dlg := EditDlg.createMultiLine(surface)
        {
            .setMessage("Press Shift+Enter to add a new line")
            .setText(curText)
            .richEdit.selectAll()
            .surface.setTitle("Enter Text")
            .surface.center(surface)
        }
        if dlg.doModal() != Dialog.BtnOk
            return curText
        capture := self.main.getCapture()
        undo := capture.newUndo(.SetText)
        undo.text = dlg.getText()
        capture.pushUndo(undo)
        return undo.text
    }

    mtd propText()
    {
        curSelType := getSelectedFormType()
        if curSelType != FormText
            return
        curSel := cast(*FormText) getSelectedBaseForm()

        stack := StackLayoutCtrl.create(mainLayout, .Top, {0, 0, 0, 160})
        stack.padding = 8

        // Font
        combo := ComboBox.create(stack)

        combo.addItem(g_App.theme.res.defaultTypeFaceR.getFamilyName())
        if !curSel.typeFace
            combo.selectedIdx = 0
        combo.addSeparator()

        sysFonts = FontFamily.enumerateFromOs()
        visit &fnt: sysFonts
        {
            combo.addItem(fnt.name, userData0: fnt)
            if curSel.fontName == fnt.name
                combo.selectedIdx = cast(u32) combo.items.count - 1
        }

        combo.sigChanged += closure|self|(combo, idx)
        {
            if idx == Swag.U32.Max
                return

            capture := self.main.getCapture()
            undo := capture.newUndo(.SetFont)

            sel := combo.items[idx]
            if sel.userData0
            {
                p := cast(*FontFamilyEnumerate) sel.userData0
                undo.text = p.name
            }

            undo.fontStyle = .Regular
            capture.pushUndo(undo)
        }

        // Text
        edt := EditBox.create(stack, curSel.text)
        edt.rightMargin = 24
        with btnedt := IconButton.create(edt, getTheme().getIcon24(.More, 20), {0, 0, 20, 20})
        {
            .dockStyle = .Right
            .sigPressed += closure|self, edt|(btn) {
                txt := self.editText(edt.text)
                edt.setText(txt)
            }
        }

        edt.name = "Enter text"
        edt.sigLoseFocus += closure|self|(edit) {
            curSel := cast(*FormText) self.getSelectedBaseForm()
            if curSel.text != edit.text
            {
                capture := self.main.getCapture()
                undo := capture.newUndo(.SetText)
                undo.text = edit.text
                capture.pushUndo(undo)
            }
        }

        // Bold/Italic
        stack2 := StackLayoutCtrl.create(stack, .HorzCenter, {0, 0, 0, 30})
        with bar2 := IconBar.create(stack2, 24)
        {
            .addCheckableToggleItem(Icon.from(&main.icons24, 16), CmdIdTextBold)
            .addCheckableToggleItem(Icon.from(&main.icons24, 17), CmdIdTextItalic)

            .sigUpdateState += closure|self|(evt) {
                curSel := cast(*FormText) self.getSelectedBaseForm()
                evt.setFlags.add(.Checked)
                evt.accepted = true

                switch evt.id
                {
                case CmdIdTextBold:    evt.checked = curSel.fontStyle == .Bold or curSel.fontStyle == .BoldItalic
                case CmdIdTextItalic:  evt.checked = curSel.fontStyle == .Italic or curSel.fontStyle == .BoldItalic
                }
            }

            .sigCheckChanged += closure|self|(btn) {
                capture := self.main.getCapture()
                curSel  := cast(*FormText) self.getSelectedBaseForm()
                style := curSel.fontStyle
                switch btn.id
                {
                case CmdIdTextBold:
                    if btn.isChecked and style == .Regular
                        style = .Bold
                    elif btn.isChecked and style == .Italic
                        style = .BoldItalic
                    elif !btn.isChecked and style == .Bold
                        style = .Regular
                    elif !btn.isChecked and style == .BoldItalic
                        style = .Italic

                case CmdIdTextItalic:
                    if btn.isChecked and style == .Regular
                        style = .Italic
                    elif btn.isChecked and style == .Bold
                        style = .BoldItalic
                    elif !btn.isChecked and style == .Italic
                        style = .Regular
                    elif !btn.isChecked and style == .BoldItalic
                        style = .Bold
                }

                undo := capture.newUndo(.SetFontStyle)
                undo.fontStyle = style
                capture.pushUndo(undo)
            }

            bar2.updateCommandState()
        }

        // Align
        stack1 := StackLayoutCtrl.create(stack, .HorzCenter, {0, 0, 0, 40})
        with bar := IconBar.create(stack1, 24)
        {
            .addCheckableItem(Icon.from(&main.icons24, 8),  CmdIdTextHLeft)
            .addCheckableItem(Icon.from(&main.icons24, 9),  CmdIdTextHCenter)
            .addCheckableItem(Icon.from(&main.icons24, 10), CmdIdTextHRight)
            .addCheckableItem(Icon.from(&main.icons24, 11), CmdIdTextVTop)
            .addCheckableItem(Icon.from(&main.icons24, 12), CmdIdTextVCenter)
            .addCheckableItem(Icon.from(&main.icons24, 13), CmdIdTextVBottom)

            .sigUpdateState += closure|self|(evt) {
                curSel := cast(*FormText) self.getSelectedBaseForm()
                evt.setFlags.add(.Checked)
                evt.accepted = true

                switch evt.id
                {
                case CmdIdTextHLeft:    evt.checked = curSel.alignHorz == .LeftTop
                case CmdIdTextHCenter:  evt.checked = curSel.alignHorz == .Center
                case CmdIdTextHRight:   evt.checked = curSel.alignHorz == .RightBottom
                case CmdIdTextVTop:     evt.checked = curSel.alignVert == .LeftTop
                case CmdIdTextVCenter:  evt.checked = curSel.alignVert == .Center
                case CmdIdTextVBottom:  evt.checked = curSel.alignVert == .RightBottom
                }
            }

            .sigCheckChanged += closure|self|(btn) {
                capture := self.main.getCapture()
                switch btn.id
                {
                case CmdIdTextHLeft:
                    self.pushTypedValue(cast(u32) FormTextAlign.LeftTop, @typeof(FormText), @nameof(FormText.alignHorz), true)
                case CmdIdTextHCenter:
                    self.pushTypedValue(cast(u32) FormTextAlign.Center, @typeof(FormText), @nameof(FormText.alignHorz), true)
                case CmdIdTextHRight:
                    self.pushTypedValue(cast(u32) FormTextAlign.RightBottom, @typeof(FormText), @nameof(FormText.alignHorz), true)
                case CmdIdTextVTop:
                    self.pushTypedValue(cast(u32) FormTextAlign.LeftTop, @typeof(FormText), @nameof(FormText.alignVert), true)
                case CmdIdTextVCenter:
                    self.pushTypedValue(cast(u32) FormTextAlign.Center, @typeof(FormText), @nameof(FormText.alignVert), true)
                case CmdIdTextVBottom:
                    self.pushTypedValue(cast(u32) FormTextAlign.RightBottom, @typeof(FormText), @nameof(FormText.alignVert), true)
                }
            }

            bar.updateCommandState()
        }
    }

    mtd propStartSize()
    {
        curSelType := getSelectedFormType()
        if curSelType != FormLine and curSelType != FormArrow
            return
        curSel := cast(*FormLine) getSelectedBaseForm()

        ctrl := createSlider("Start Size", acast MinCapSize, acast MaxCapSize, curSel.startSize)
        ctrl.slider.sigStartChange += closure|self|(slider) {
            self.pushTypedValue(cast,bit(u32) slider.getValue(), @typeof(FormLine), @nameof(FormLine.startSize), true)
        }
        ctrl.slider.sigChanged += closure|self|(slider) {
            self.pushTypedValue(cast,bit(u32) slider.getValue(), @typeof(FormLine), @nameof(FormLine.startSize), false)
        }
    }

    mtd propEndSize()
    {
        curSelType := getSelectedFormType()
        if curSelType != FormLine and curSelType != FormArrow
            return

        curSel := cast(*FormLine) getSelectedBaseForm()
        ctrl := createSlider("End Size", acast MinCapSize, acast MaxCapSize, curSel.endSize)
        ctrl.slider.sigStartChange += closure|self|(slider) {
            self.pushTypedValue(cast,bit(u32) slider.getValue(), @typeof(FormLine), @nameof(FormLine.endSize), true)
        }
        ctrl.slider.sigChanged += closure|self|(slider) {
            self.pushTypedValue(cast,bit(u32) slider.getValue(), @typeof(FormLine), @nameof(FormLine.endSize), false)
        }
    }

    mtd propRoundSize()
    {
        curSelType := getSelectedFormType()
        if curSelType != FormShape
            return

        curSel := cast(*FormShape) getSelectedBaseForm()
        if curSel.kind != .RoundRectangle
            return

        ctrl := createSlider("Round", acast MinRoundSize, acast MaxRoundSize, curSel.roundSize)
        ctrl.slider.sigStartChange += closure|self|(slider) {
            self.pushTypedValue(cast,bit(u32) slider.getValue(), @typeof(FormShape), @nameof(FormShape.roundSize), true)
        }
        ctrl.slider.sigChanged += closure|self|(slider) {
            self.pushTypedValue(cast,bit(u32) slider.getValue(), @typeof(FormShape), @nameof(FormShape.roundSize), false)
        }
    }

    mtd propBorderSize()
    {
        curSelType := getSelectedFormType()
        if curSelType == FormStep
            return

        curSel := getSelectedBaseForm()
        if curSel.subKind == .PixSel
            return

        minS := MinBorderSize
        maxS := MaxBorderSize
        labl := "Tickness"
        if curSelType == FormText
        {
            minS = MinTextBorderSize
            maxS = MaxTextBorderSize
            labl = "Outline"
        }
        elif curSelType == FormDraw
        {
            minS = Math.max(minS, 1)
        }

        ctrl := createSlider(labl, acast minS, acast maxS, curSel.borderSize)
        ctrl.slider.sigStartChange += closure|self|(slider) {
            self.pushTypedValue(cast,bit(u32) slider.getValue(), @typeof(BaseForm), @nameof(BaseForm.borderSize), true)
        }
        ctrl.slider.sigChanged += closure|self|(slider) {
            self.pushTypedValue(cast,bit(u32) slider.getValue(), @typeof(BaseForm), @nameof(BaseForm.borderSize), false)
        }
    }

    mtd propFontSize()
    {
        curSelType := getSelectedFormType()
        if curSelType != FormText
            return

        curSel := cast(*FormText) getSelectedBaseForm()
        ctrl := createSlider("Size", acast MinFontSize, acast MaxFontSize, acast curSel.fontSize)
        ctrl.slider.sigStartChange += closure|self|(slider) {
            self.pushTypedValue(cast,bit(u32) slider.getValue(), @typeof(FormText), @nameof(FormText.fontSize), true)
        }
        ctrl.slider.sigChanged += closure|self|(slider) {
            self.pushTypedValue(cast,bit(u32) slider.getValue(), @typeof(FormText), @nameof(FormText.fontSize), false)
        }
    }

    mtd propBoldSize()
    {
        curSelType := getSelectedFormType()
        if curSelType != FormText
            return

        curSel := cast(*FormText) getSelectedBaseForm()
        ctrl := createSlider("Strengh", acast MinBoldSize, acast MaxBoldSize, acast curSel.boldSize)
        ctrl.slider.sigStartChange += closure|self|(slider) {
            self.pushTypedValue(cast,bit(u32) slider.getValue(), @typeof(FormText), @nameof(FormText.boldSize), true)
        }
        ctrl.slider.sigChanged += closure|self|(slider) {
            self.pushTypedValue(cast,bit(u32) slider.getValue(), @typeof(FormText), @nameof(FormText.boldSize), false)
        }
    }

    mtd propOpacity()
    {
        curSel := getSelectedBaseForm()
        if curSel.subKind == .PixSel
            return

        ctrl := createSlider("Opacity", 0, 100, acast curSel.alpha)
        ctrl.slider.sigStartChange += closure|self|(slider) {
            self.pushTypedValue(cast(u32) slider.getValue(), @typeof(BaseForm), @nameof(BaseForm.alpha), true)
        }
        ctrl.slider.sigChanged += closure|self|(slider) {
            self.pushTypedValue(cast(u32) slider.getValue(), @typeof(BaseForm), @nameof(BaseForm.alpha), false)
        }
    }

    mtd propImageKind()
    {
        curSelType := getSelectedFormType()
        if curSelType != FormImage
            return

        curSel := cast(*FormImage) getSelectedBaseForm()

        ctrl := ComboCtrl.create(mainLayout, "Kind")
        ctrl.comboSize = 210
        with ctrl.combo
        {
            visit v: @typeof(FormImageKind).values
                .addItem(v.name)
            .selectItem(cast(u32) curSel.kind)
            .sigChanged += closure|self|(combo, val) {
                self.pushTypedValue(val, @typeof(FormImage), @nameof(FormImage.kind), true)
                self.selectionHasChanged()
            }
        }

        ctrl1 := ComboCtrl.create(mainLayout, "Place")
        ctrl1.comboSize = 210
        with ctrl1.combo
        {
            visit v: @typeof(FormImagePlacement).values
                .addItem(v.name)
            .selectItem(cast(u32) curSel.place)
            .sigChanged += closure|self|(combo, val) {
                self.pushTypedValue(val, @typeof(FormImage), @nameof(FormImage.place), true)
            }
        }
    }

    mtd propStrength()
    {
        curSelType := getSelectedFormType()
        if curSelType != FormImage
            return
        curSel := cast(*FormImage) getSelectedBaseForm()
        if curSel.kind == .Image
            return

        ctrl := createSlider("Strength", 0, 100, acast curSel.strength)
        ctrl.slider.sigStartChange += closure|self|(slider) {
            self.pushTypedValue(cast(u32) slider.getValue(), @typeof(FormImage), @nameof(FormImage.strength), true)
        }
        ctrl.slider.sigChanged += closure|self|(slider) {
            self.pushTypedValue(cast(u32) slider.getValue(), @typeof(FormImage), @nameof(FormImage.strength), false)
        }
    }

    mtd propJoinStyle()
    {
        curSelType := getSelectedFormType()
        if curSelType != FormText and curSelType != FormPolygon
            return
        curSel := getSelectedBaseForm()

        ctrl := ComboCtrl.create(mainLayout, "Joins")
        ctrl.comboSize = 210
        with ctrl.combo
        {
            .addItem("Bevel")
            .addItem("Round")
            .addItem("Miter")
            .addItem("MiterBevel")
            .selectItem(cast(u32) curSel.joinStyle - 1)
            .sigChanged += closure|self|(combo, val) {
                v := val + 1
                self.pushTypedValue(acast v, @typeof(BaseForm), @nameof(BaseForm.joinStyle), true)
            }
        }
    }

    mtd propShadow()
    {
        curSel := getSelectedBaseForm()
        if curSel.subKind == .PixSel
            return

        ctrl := ComboCtrl.create(mainLayout, "Shadow")
        ctrl.comboSize = 210
        with ctrl.combo
        {
            .addItem("No Shadow")
            .addItem("Small")
            .selectItem(curSel.paintShadow ? 1 : 0)
            .sigChanged += closure|self|(combo, val) {
                b := val == 1 ? true : false
                self.pushTypedValue(acast b, @typeof(BaseForm), @nameof(BaseForm.paintShadow), true)
            }
        }
    }

    mtd propPixSel()
    {
        curSel := getSelectedBaseForm()
        if curSel.subKind != .PixSel
            return

        discard PushButton.create(mainLayout, "", id: CmdIdPixSelFill)
        discard PushButton.create(mainLayout, "", id: CmdIdPixSelSmartFill)
        discard PushButton.create(mainLayout, "", id: CmdIdPixSelClear)
        discard PushButton.create(mainLayout, "", id: CmdIdPixSelSmartMove)
    }

    mtd selectionHasChanged()
    {
        visit it: mainLayout.childs
            it.destroy()
        if !getSelectedBaseForm()
            return

        stack0 := StackLayoutCtrl.create(mainLayout, .HorzCenter, {0, 0, 0, BtnColorHeight + 12})
        propBackColor(stack0)
        propBorderColor(stack0)
        propShape(stack0)
        propStep(stack0)
        if stack0.childs.count == 0
            stack0.destroyNow()

        stack1 := StackLayoutCtrl.create(mainLayout, .HorzCenter, {0, 0, 0, BtnStyleHeight + 12})
        propStartCapStyle(stack1)
        propDashStyle(stack1)
        propEndCapStyle(stack1)
        if stack1.childs.count == 0
            stack1.destroyNow()

        propImageKind()
        propStrength()
        propNextStep()
        propText()
        propFontSize()
        propBoldSize()
        propBorderSize()
        propStartSize()
        propEndSize()
        propRoundSize()
        propOpacity()
        propJoinStyle()
        propShadow()
        propPixSel()

        mainLayout.computeLayout()
        if mainLayout.childs.count
            setScrollSize(0, mainLayout.childs.back().position.bottom() + 16)
        else
            setScrollSize(0, 0)

        capture := main.getCapture()
        if capture
            capture.editView.setFocus()
    }

    func create(parent: *Wnd)->*PropWnd
    {
        with prop := Wnd.create'PropWnd(parent, {0, 0, 0, 300})
        {
            .backgroundStyle = .Window
            .scrollWndFlags |= .ForceVertical | .SmallBar | .TransparentBar

            .mainLayout = StackLayoutCtrl.create(prop)
            .mainLayout.layoutKind = .Top
            .mainLayout.padding = 8
            .mainLayout.padding.y = 16

            .setView(.mainLayout)
        }

        return prop
    }
}