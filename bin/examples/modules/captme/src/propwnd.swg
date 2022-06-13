const MinBorderSize     = 1.0
const MaxBorderSize     = 50.0
const MinTextBorderSize = 1.0
const MaxTextBorderSize = 10.0
const MinCapSize        = 1.0
const MaxCapSize        = 8.0
const MinFontSize       = 10.0
const MaxFontSize       = 150.0
const MinBoldSize       = 0.0
const MaxBoldSize       = 4.0

const CapSize         = 8.0
const DashSize        = 8.0
const BtnColorWidth   = 68.0
const BtnColorHeight  = 88.0
const BtnStyleWidth   = 88.0
const BtnStyleHeight  = 68.0

struct PropWnd
{
    using layoutWnd:    StackLayoutCtrl
    main:               *MainWnd
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
            case Polygon:
                return cast(*BaseForm) &main.mdlFormPolygon
            case Text:
                return cast(*BaseForm) &main.mdlFormText
            case Favorites:
                return @dataof(main.favoriteMdl)
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
            case Polygon:
                return FormPolygon
            case Text:
                return FormText
            case Favorites:
                return @kindof(main.favoriteMdl)
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
        ctrl := SliderCtrl.create(self, #self.name)
        ctrl.edit.setForm(.Square)
        ctrl.setMinMaxS64(minv, maxv)
        ctrl.dockStyle = .Top
        ctrl.setValue(val)
        return ctrl
    }

    func createArrowButton(stack: *Wnd, w, h: f32)->*IconButton
    {
        btn := IconButton.create(stack, @{}, @{0, 0, w, h})
        btn.setForm(.RoundSquare)
        btn.style.addStyleSheetColors("btnIcon_Bk        $lighterMedium")
        btn.style.addStyleSheetColors("btnIcon_HotBk     $lighterBig")
        btn.style.addStyleSheetColors("btnIcon_PressedBk $lighterVeryBig")
        btn.iconBtnFlags = .Popup | .CheckableToggle
        btn.arrowPos = .Bottom
        btn.textPos = .Top
        btn.checkedForm = .None
        return btn
    }

    func createPopup(btn: *IconButton, w, h: f32)->*FrameWnd
    {
        pos     := btn.getSurfaceRect()
        topView := btn.getTopView()
        pt      := topView.surfaceToLocal(@{pos.x, pos.bottom()})
        pt.y += btn.getThemeMetrics().frameWnd_AnchorHeight

        frame := Wnd.create'FrameWnd(topView, @{pt.x, pt.y, w, h})
        frame.setFrameForm(.Round)
        frame.setFrameFlags(FrameWndFlags.SmallShadow)
        frame.style.addStyleSheetColors("frameWnd_Border $frameWnd_PopupBorder")

        sdw := btn.getThemeMetrics().frameWnd_SmallShadowSize
        frame.fitPosInParent(sdw.z, sdw.w)
        frame.anchor = .Top
        frame.anchorPos = pos.horzCenter() - frame.getSurfaceRect().x

        btn.popup = frame
        return frame
    }

    func createColorPicker(btn: *IconButton, color: Color)->*ColorPickerCtrl
    {
        frame := createPopup(btn, 356, 382)

        picker := ColorPickerCtrl.create(frame, @{8, 54})
        picker.setColor(color)

        palette := PalettePicker.create(frame, @{16, 16, 340, 48})
        palette.addColor(Argb.White)
        palette.addColor(Argb.Gray)
        palette.addColor(Argb.Black)
        palette.addColor(Argb.Red)
        palette.addColor(Argb.Green)
        palette.addColor(Argb.Blue)
        palette.addColor(Argb.Yellow)
        palette.addColor(Argb.Purple)
        palette.addColor(Color.fromArgb(0))
        palette.boxSize = 32
        palette.sigChanged += @|picker|(pal) {
            picker.setColor(pal.selectedColor)
        }

        return picker
    }

    mtd createPopupShapeKind(btn: *IconButton)
    {
        frame := createPopup(btn, 180, 66)
        layout := frame.createView'WrapLayoutCtrl()
        layout.wrapWidth, layout.wrapHeight = 48
        layout.padding = 4

        loop i: @countof(FormShapeKind)
        {
            btn0 := IconButton.create(frame.view)
            btn0.userData = cast(*void) i
            btn0.setForm(.RoundSquare)
            me := self
            btn0.sigPaintIcon = @|me|(btn, bc, pos)
            {
                bc.painter.setParams(.Antialiased)
                curSel := cast(*FormShape) me.getSelectedBaseForm()
                rect := pos
                rect.inflate(-9, -9)
                rect.x = rect.horzCenter() - rect.height * 0.5
                rect.width = rect.height
                color := btn.getThemeColors().btnIcon_Icon

                #[Swag.Complete]
                switch cast(FormShapeKind) cast(uint) btn.userData
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

            btn0.sigPressed += @|me|(btn) {
                kind := cast(u32) cast(FormShapeKind) cast(uint) btn.userData
                me.pushTypedValue(kind, @typeof(FormShape), @nameof(FormShape.kind), true)
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
                cap := cast(LineCapStyle) cast(uint) btn.userData
                btn.isChecked = curSel.beginCap == cap
                btn.invalidate()
            }
        }

        curSel := cast(*FormLine) getSelectedBaseForm()
        visit i: LineCapStyle
        {
            btn0 := IconButton.create(layout, @{}, @{0, 0, 0, 40})
            btn0.userData = cast(*void) cast(uint) i
            btn0.setForm(.RoundSquare)
            btn0.isChecked = curSel.beginCap == i

            me := self
            btn0.sigPaintIcon = @|me|(btn, bc, pos)
            {
                bc.painter.setParams(.Antialiased)
                curSel := cast(*FormShape) me.getSelectedBaseForm()
                rect := pos
                rect.inflate(-9, -9)
                cap := cast(LineCapStyle) cast(uint) btn.userData
                pen := Pen.createSolid(btn.getThemeColors().btnIcon_Icon, CapSize)
                pen.beginCapStyle = cap
                pen.endCapStyle = .None
                rect.moveLeft(FormLine.drawCapOffset(cap, CapSize, pen.capStartScaleX))
                bc.painter.drawLine(rect.x, rect.vertCenter(), rect.right(), rect.vertCenter(), pen)
            }

            btn0.sigPressed += @|me, layout|(btn) {
                kind := cast(u32) cast(LineCapStyle) cast(uint) btn.userData
                me.pushTypedValue(kind, @typeof(FormLine), @nameof(FormLine.beginCap), true)
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
                cap := cast(LineCapStyle) cast(uint) btn.userData
                btn.isChecked = curSel.endCap == cap
                btn.invalidate()
            }
        }

        curSel := cast(*FormLine) getSelectedBaseForm()
        visit i: LineCapStyle
        {
            btn0 := IconButton.create(layout, @{}, @{0, 0, 0, 40})
            btn0.userData = cast(*void) cast(uint) i
            btn0.setForm(.RoundSquare)
            btn0.isChecked = curSel.endCap == i

            me := self
            btn0.sigPaintIcon = @|me|(btn, bc, pos)
            {
                bc.painter.setParams(.Antialiased)
                curSel := cast(*FormShape) me.getSelectedBaseForm()
                rect := pos
                rect.inflate(-9, -9)
                cap := cast(LineCapStyle) cast(uint) btn.userData
                pen := Pen.createSolid(btn.getThemeColors().btnIcon_Icon, CapSize)
                pen.beginCapStyle = .None
                pen.endCapStyle = cap
                rect.moveRight(-FormLine.drawCapOffset(cap, CapSize, pen.capStartScaleX))
                bc.painter.drawLine(rect.x, rect.vertCenter(), rect.right(), rect.vertCenter(), pen)
            }

            btn0.sigPressed += @|me, layout|(btn) {
                kind := cast(u32) cast(LineCapStyle) cast(uint) btn.userData
                me.pushTypedValue(kind, @typeof(FormLine), @nameof(FormLine.endCap), true)
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
                dash := cast(DashStyle) cast(uint) btn.userData
                btn.isChecked = curSel.dashStyle == dash
                btn.invalidate()
            }
        }

        curSel := getSelectedBaseForm()
        visit i: DashStyle
        {
            if i == .Pattern continue

            btn0 := IconButton.create(layout, @{}, @{0, 0, 0, 32})
            btn0.userData = cast(*void) cast(uint) i
            btn0.setForm(.RoundSquare)
            btn0.isChecked = curSel.dashStyle == i

            me := self
            btn0.sigPaintIcon = @|me|(btn, bc, pos)
            {
                bc.painter.setParams(.Antialiased)
                curSel := cast(*FormShape) me.getSelectedBaseForm()
                rect := pos
                rect.inflate(-9, -9)
                dash := cast(DashStyle) cast(uint) btn.userData
                pen := Pen.createDash(dash, btn.getThemeColors().btnIcon_Icon, DashSize)
                bc.painter.drawLine(rect.x, rect.vertCenter(), rect.right(), rect.vertCenter(), pen)
            }

            btn0.sigPressed += @|me, layout|(btn) {
                kind := cast(u32) cast(DashStyle) cast(uint) btn.userData
                me.pushTypedValue(kind, @typeof(BaseForm), @nameof(BaseForm.dashStyle), true)
                me.updateBtnState(layout)
            }
        }

        layout.computeLayout()
    }

    mtd propBorderColor(stack: *Wnd)
    {
        btn := createArrowButton(stack, BtnColorWidth, BtnColorHeight)

        curSelType := getSelectedFormType()
        switch curSelType
        {
        case FormLine, FormArrow:
            btn.name = "Color"
        default:
            btn.name = "Outline"
        }

        me := self
        btn.sigPaintIcon = @|me|(btn, bc, pos)
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

        btn.sigShowPopup += @|me|(btn) {
            curSel := me.getSelectedBaseForm()
            picker := createColorPicker(btn, curSel.borderColor)
            me.pushTypedValue(curSel.borderColor.argb, @typeof(BaseForm), @nameof(BaseForm.borderColor), true)
            picker.sigChanged += @|me, btn|(pk) {
                me.pushTypedValue(pk.getColor().argb, @typeof(BaseForm), @nameof(BaseForm.borderColor), false)
                btn.invalidate()
            }
        }
    }

    mtd propBackColor(stack: *Wnd)
    {
        curSelType := getSelectedFormType()
        if curSelType == FormLine or curSelType == FormArrow
            return

        btn := createArrowButton(stack, BtnColorWidth, BtnColorHeight)
        btn.name = "Fill"
        me := self
        btn.sigPaintIcon = @|me|(btn, bc, pos)
        {
            bc.painter.setParams(.Antialiased)
            curSel := me.getSelectedBaseForm()
            rect := pos
            rect.x = rect.horzCenter() - 21
            rect.y = rect.vertCenter() - 21
            rect.width, rect.height = 42
            if !curSel.backColor.isOpaque()
                bc.painter.fillRoundRect(rect, 10, 10, Brush.createHatch(.CheckBoardLarge, Argb.DarkGray))
            bc.painter.fillRoundRect(rect, 10, 10, curSel.backColor)
        }

        btn.sigShowPopup += @|me|(btn) {
            curSel := me.getSelectedBaseForm()
            picker := createColorPicker(btn, curSel.backColor)
            me.pushTypedValue(curSel.backColor.argb, @typeof(BaseForm), @nameof(BaseForm.backColor), true)
            picker.sigChanged += @|me, btn|(pk) {
                me.pushTypedValue(pk.getColor().argb, @typeof(BaseForm), @nameof(BaseForm.backColor), false)
                btn.invalidate()
            }
        }
    }

    mtd propShape(stack: *Wnd)
    {
        curSelType := getSelectedFormType()
        if curSelType != FormShape
            return

        btn := createArrowButton(stack, BtnColorWidth, BtnColorHeight)
        btn.name = "Shape"
        me := self
        btn.sigPaintIcon = @|me|(btn, bc, pos)
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

        btn.sigShowPopup += @|me|(btn) {
            curSel := cast(*FormShape) me.getSelectedBaseForm()
            me.createPopupShapeKind(btn)
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
        btn.sigPaintIcon = @|me|(btn, bc, pos)
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

        btn.sigShowPopup += @|me|(btn) {
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
        btn.sigPaintIcon = @|me|(btn, bc, pos)
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

        btn.sigShowPopup += @|me|(btn) {
            curSel := cast(*FormShape) me.getSelectedBaseForm()
            me.createPopupEndCapStyle(btn)
        }
    }

    mtd propDashStyle(stack: *Wnd)
    {
        curSelType := getSelectedFormType()
        if curSelType != FormShape and curSelType != FormLine and curSelType != FormArrow
            return

        btn := createArrowButton(stack, BtnStyleWidth, BtnStyleHeight)
        btn.name = "Dash"
        me := self
        btn.sigPaintIcon = @|me|(btn, bc, pos)
        {
            bc.painter.setParams(.Antialiased)
            curSel := me.getSelectedBaseForm()
            rect := pos
            rect.inflate(-9, -9)
            pen := Pen.createDash(curSel.dashStyle, btn.getThemeColors().btnIcon_Icon, DashSize)
            bc.painter.drawLine(rect.x, rect.vertCenter(), rect.right(), rect.vertCenter(), pen)
        }

        btn.sigShowPopup += @|me|(btn) {
            curSel := cast(*FormShape) me.getSelectedBaseForm()
            me.createPopupDashStyle(btn)
        }
    }

    mtd propText()
    {
        curSelType := getSelectedFormType()
        if curSelType != FormText
            return

        curSel := cast(*FormText) getSelectedBaseForm()

        edt := EditBox.create(self, curSel.text)
        edt.sigLoseFocus += @|self|(edit) {
            capture := self.main.getCapture()
            undo := capture.newUndo(.SetText)
            undo.text = edit.text
            capture.pushUndo(undo)
        }
    }

    mtd propStartSize()
    {
        curSelType := getSelectedFormType()
        if curSelType != FormLine and curSelType != FormArrow
            return
        curSel := cast(*FormLine) getSelectedBaseForm()

        ctrl := createSlider("Start Size", acast MinCapSize, acast MaxCapSize, curSel.startSize)
        ctrl.slider.sigStartChange += @|self|(slider) {
            self.pushTypedValue(bitcast(u32) slider.getValue(), @typeof(FormLine), @nameof(FormLine.startSize), true)
        }
        ctrl.slider.sigChanged += @|self|(slider) {
            self.pushTypedValue(bitcast(u32) slider.getValue(), @typeof(FormLine), @nameof(FormLine.startSize), false)
        }
    }

    mtd propEndSize()
    {
        curSelType := getSelectedFormType()
        if curSelType != FormLine and curSelType != FormArrow
            return

        curSel := cast(*FormLine) getSelectedBaseForm()
        ctrl := createSlider("End Size", acast MinCapSize, acast MaxCapSize, curSel.endSize)
        ctrl.slider.sigStartChange += @|self|(slider) {
            self.pushTypedValue(bitcast(u32) slider.getValue(), @typeof(FormLine), @nameof(FormLine.endSize), true)
        }
        ctrl.slider.sigChanged += @|self|(slider) {
            self.pushTypedValue(bitcast(u32) slider.getValue(), @typeof(FormLine), @nameof(FormLine.endSize), false)
        }
    }

    mtd propBorderSize()
    {
        curSel := getSelectedBaseForm()

        minS := MinBorderSize
        maxS := MaxBorderSize
        labl := "Tickness"
        if getSelectedFormType() == FormText
        {
            minS = MinTextBorderSize
            maxS = MaxTextBorderSize
            labl = "Outline"
        }

        ctrl := createSlider(labl, acast minS, acast maxS, curSel.borderSize)
        ctrl.slider.sigStartChange += @|self|(slider) {
            self.pushTypedValue(bitcast(u32) slider.getValue(), @typeof(BaseForm), @nameof(BaseForm.borderSize), true)
        }
        ctrl.slider.sigChanged += @|self|(slider) {
            self.pushTypedValue(bitcast(u32) slider.getValue(), @typeof(BaseForm), @nameof(BaseForm.borderSize), false)
        }
    }

    mtd propFontSize()
    {
        curSelType := getSelectedFormType()
        if curSelType != FormText
            return

        curSel := cast(*FormText) getSelectedBaseForm()
        ctrl := createSlider("Size", acast MinFontSize, acast MaxFontSize, acast curSel.fontSize)
        ctrl.slider.sigStartChange += @|self|(slider) {
            self.pushTypedValue(bitcast(u32) slider.getValue(), @typeof(FormText), @nameof(FormText.fontSize), true)
        }
        ctrl.slider.sigChanged += @|self|(slider) {
            self.pushTypedValue(bitcast(u32) slider.getValue(), @typeof(FormText), @nameof(FormText.fontSize), false)
        }
    }

    mtd propBoldSize()
    {
        curSelType := getSelectedFormType()
        if curSelType != FormText
            return

        curSel := cast(*FormText) getSelectedBaseForm()
        ctrl := createSlider("Strengh", acast MinBoldSize, acast MaxBoldSize, acast curSel.boldSize)
        ctrl.slider.sigStartChange += @|self|(slider) {
            self.pushTypedValue(bitcast(u32) slider.getValue(), @typeof(FormText), @nameof(FormText.boldSize), true)
        }
        ctrl.slider.sigChanged += @|self|(slider) {
            self.pushTypedValue(bitcast(u32) slider.getValue(), @typeof(FormText), @nameof(FormText.boldSize), false)
        }
    }

    mtd propOpactiy()
    {
        curSel := getSelectedBaseForm()
        ctrl := createSlider("Opacity", 0, 100, acast curSel.alpha)
        ctrl.slider.sigStartChange += @|self|(slider) {
            self.pushTypedValue(cast(u32) slider.getValue(), @typeof(BaseForm), @nameof(BaseForm.alpha), true)
        }
        ctrl.slider.sigChanged += @|self|(slider) {
            self.pushTypedValue(cast(u32) slider.getValue(), @typeof(BaseForm), @nameof(BaseForm.alpha), false)
        }
    }

    mtd selectionHasChanged()
    {
        while childs.count
            childs[0].destroyNow()
        if !getSelectedBaseForm()
            return

        stack0 := StackLayoutCtrl.create(self, .HorzCenter, @{0, 0, 0, BtnColorHeight + 12})
        propBackColor(stack0)
        propBorderColor(stack0)
        propShape(stack0)
        if stack0.childs.count == 0
            stack0.destroyNow()

        stack1 := StackLayoutCtrl.create(self, .HorzCenter, @{0, 0, 0, BtnStyleHeight + 12})
        propStartCapStyle(stack1)
        propDashStyle(stack1)
        propEndCapStyle(stack1)
        if stack1.childs.count == 0
            stack1.destroyNow()

        propText()
        propBorderSize()
        propStartSize()
        propEndSize()
        propOpactiy()
        propFontSize()
        propBoldSize()

        layoutWnd.padding = 4
        layoutWnd.padding.y = 16
        layoutWnd.computeLayout()

        capture := main.getCapture()
        if capture
            capture.editView.setFocus()
    }
}