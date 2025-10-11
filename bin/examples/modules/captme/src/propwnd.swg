const BtnColorWidth  = 68.0
const BtnColorHeight = 88.0
const BtnStyleWidth  = 88.0
const BtnStyleHeight = 68.0

const CapSize  = 8.0
const DashSize = 8.0

const CmdIdTextHLeft   = "CmdIdTextHLeft"
const CmdIdTextHCenter = "CmdIdTextHCenter"
const CmdIdTextHRight  = "CmdIdTextHRight"
const CmdIdTextVTop    = "CmdIdTextVTop"
const CmdIdTextVCenter = "CmdIdTextVCenter"
const CmdIdTextVBottom = "CmdIdTextVBottom"
const CmdIdTextBold    = "CmdIdTextBold"
const CmdIdTextItalic  = "CmdIdTextItalic"

struct PropWnd
{
    using scrollWnd:     ScrollWnd
    mainLayout:          *StackLayoutCtrl
    main:                *MainWnd
    sysFonts:            Array'FontFamilyEnumerate
}

impl PropWnd
{
    mtd hasRealSelection()->bool
    {
        let capture = .main.getCapture()
        if !capture or capture.selection.count == 0 do
            return false
        return true
    }

    mtd getSelectedBaseForm()->*BaseForm
    {
        let capture = .main.getCapture()
        if !capture or capture.selection.count == 0
        {
            let iform = assume .main.editModeToModel(.main.editMode)
            return cast(*BaseForm) @dataof(iform)
        }

        return assume capture.getSelectedBaseForm()
    }

    mtd getSelectedFormType()->typeinfo
    {
        let capture = .main.getCapture()
        if !capture or capture.selection.count == 0
        {
            let iform = assume .main.editModeToModel(.main.editMode)
            return @kindof(iform)
        }

        return assume capture.getSelectedFormType()
    }

    mtd pushTypedValue(val: u32, type: typeinfo, name: string, first: bool, common = false)
    {
        let capture = .main.getCapture()

        // Change model
        if !capture or !capture.selection.count
        {
            let form   = .getSelectedBaseForm()
            let field  = assume Reflection.getField(type, name)
            var valU32 = val
            Reflection.setFieldValue(form, field, &valU32, true)
        }

        // Change selection
        else
        {
            if !first do
                capture.undo()
            let undo = capture.newUndo(.SetTypedValue)
            undo.valU32 = val
            if !common do
                undo.mdlType = type
            undo.valRef = Reflection.getField(type, name)
            capture.pushUndo(undo)
        }

        .main.quickWnd.updateSelection()
        .main.editWnd.invalidate()
    }

    #[Swag.Overload]
    mtd createSlider(name: string, minv, maxv: s64, val: f32)->*SliderCtrl
    {
        with let ctrl = SliderCtrl.create(me.mainLayout, name)
        {
            .setMinMaxS64(minv, maxv)
            .dockStyle = .Top
            .setValue(val)
        }

        return ctrl
    }

    func createArrowButton(stack: *Wnd, w, h: f32)->*IconButton
    {
        with let btn = IconButton.create(stack, {}, {0, 0, w, h})
        {
            .setForm(.RoundSquare)
            .style.addStyleSheetColors("btnIcon_Bk                  $lighterMedium")
            .style.addStyleSheetColors("btnIcon_HotBk               $lighterBig")
            .style.addStyleSheetColors("btnIcon_PressedBk           $lighterVeryBig")
            .style.addStyleSheetColors("btnIcon_CheckedPressedIcon  $btnIcon_Text")
            .style.addStyleSheetColors("btnIcon_CheckedPressedText  $btnIcon_Text")

            .iconBtnFlags.add(.Popup | .CheckableToggle)
            .arrowPos    = .Bottom
            .textPos     = .Top
            .checkedForm = .None
        }

        return btn
    }

    public func createColorPicker(btn: *IconButton, color: Color)->*ColorPickerCtrl
    {
        let frame = btn.createAnchorPopup(356, 382)
        with var pal: Array'Color
        {
            .add(Argb.White)
            .add(Argb.Gray)
            .add(Argb.Black)
            .add(ColorRed)
            .add(ColorGreen)
            .add(ColorBlue)
            .add(ColorYellow)
            .add(Argb.Purple)
        }

        let picker = ColorPickerCtrl.createWithPalette(frame, {8, 54}, pal)
        picker.setColor(color)
        return picker
    }

    mtd createPopupShapeKind(btn: *IconButton)
    {
        let frame  = btn.createAnchorPopup(180, 66)
        let layout = frame.createView'WrapLayoutCtrl()
        layout.wrapWidth, layout.wrapHeight = 48
        layout.padding = 4

        mtd updateBtnState(layout: *Wnd)
        {
            let curSel = cast(*FormShape) .getSelectedBaseForm()
            foreach c in layout.childs
            {
                let btn  = cast(*IconButton) c
                let kind = cast(FormShapeKind) cast(u64) btn.userData
                btn.isChecked = curSel.kind == kind
                btn.invalidate()
            }
        }

        let curSel = cast(*FormShape) .getSelectedBaseForm()

        for i in @countof(FormShapeKind)
        {
            let btn0 = IconButton.create(frame.view, {})
            btn0.userData = cast(*void) cast(u64) i
            btn0.setForm(.RoundSquare)
            if curSel.kind == i do
                btn0.isChecked = true

            btn0.sigPaintIcon = mtd||(btn, bc, pos, flags)
            {
                bc.painter.setParams(.Antialiased)
                var curSel = cast(*FormShape) .getSelectedBaseForm()
                var rect   = pos
                rect.inflate(-9, -9)
                rect.x     = rect.horzCenter() - rect.height * 0.5
                rect.width = rect.height
                let color = flags & .Checked ? btn.getThemeColors().btnIcon_CheckedIcon : btn.getThemeColors().btnIcon_Icon

                #[Swag.Complete]
                switch cast(FormShapeKind) cast(u64) btn.userData
                {
                case Ellipse:
                    let radius = rect.width * 0.5
                    bc.painter.drawEllipse(pos.horzCenter(), pos.vertCenter(), radius, radius, color, 5)
                case Rectangle:
                    bc.painter.drawRect(rect, color, 5)
                case RoundRectangle:
                    bc.painter.drawRoundRect(rect, 5, 5, color, 5)
                }
            }

            btn0.sigPressed += mtd|layout|(btn)
            {
                var kind = cast(u64) btn.userData
                .pushTypedValue(cast() kind, FormShape, #nameof(FormShape.kind), true)
                .updateBtnState(layout)
            }
        }

        layout.computeLayout()
    }

    mtd createPopupStepKind(btn: *IconButton)
    {
        let frame  = btn.createAnchorPopup(180, 66)
        let layout = frame.createView'WrapLayoutCtrl()
        layout.wrapWidth, layout.wrapHeight = 48
        layout.padding = 4

        for i in @countof(FormStepKind)
        {
            let btn0 = IconButton.create(frame.view, {})
            btn0.userData = cast(*void) cast(u64) i
            btn0.setForm(.RoundSquare)
            btn0.sigPaintIcon = mtd||(btn, bc, pos, flags)
            {
                bc.painter.setParams(.Antialiased)
                var curSel = cast(*FormStep) .getSelectedBaseForm()
                var rect   = pos
                rect.inflate(-9, -9)
                rect.x     = rect.horzCenter() - rect.height * 0.5
                rect.width = rect.height
                var color = btn.getThemeColors().btnIcon_Icon

                #[Swag.Complete]
                switch cast(FormStepKind) cast(u64) btn.userData
                {
                case Ellipse:
                    let radius = rect.width * 0.5
                    bc.painter.drawEllipse(pos.horzCenter(), pos.vertCenter(), radius, radius, color, 5)
                case Rectangle:
                    bc.painter.drawRect(rect, color, 5)
                case RoundRectangle:
                    bc.painter.drawRoundRect(rect, 5, 5, color, 5)
                }
            }

            btn0.sigPressed += mtd||(btn)
            {
                var kind = cast(u64) btn.userData
                .pushTypedValue(cast() kind, FormStep, #nameof(FormStep.kind), true)
            }
        }

        layout.computeLayout()
    }

    mtd createPopupStartCapStyle(btn: *IconButton)
    {
        let frame  = btn.createAnchorPopup(100, @countof(LineCapStyle) * 46.0)
        let layout = StackLayoutCtrl.create(frame, .Top)
        frame.setView(layout)

        mtd updateBtnState(layout: *Wnd)
        {
            let curSel = cast(*FormLine) .getSelectedBaseForm()
            foreach c in layout.childs
            {
                let btn = cast(*IconButton) c
                let cap = cast(LineCapStyle) cast(u64) btn.userData
                btn.isChecked = curSel.beginCap == cap
                btn.invalidate()
            }
        }

        let curSel = cast(*FormLine) .getSelectedBaseForm()
        foreach i in LineCapStyle
        {
            let btn0 = IconButton.create(layout, {}, {0, 0, 0, 40})
            btn0.userData = cast(*void) cast(u64) i
            btn0.setForm(.RoundSquare)
            btn0.isChecked = curSel.beginCap == i

            btn0.sigPaintIcon = mtd||(btn, bc, pos, flags)
            {
                bc.painter.setParams(.Antialiased)
                var curSel = cast(*FormShape) .getSelectedBaseForm()
                var rect   = pos
                rect.inflate(-9, -9)
                var cap   = cast(LineCapStyle) cast(u64) btn.userData
                let color = flags & .Checked ? btn.getThemeColors().btnIcon_CheckedIcon : btn.getThemeColors().btnIcon_Icon
                var pen   = Pen.createSolid(color, CapSize)
                pen.begCapStyle = cap
                pen.endCapStyle = .None
                var (w0, l0) = Painter.capStyleExtent(cap, CapSize, pen.capBegScaleX)
                rect.moveLeft(l0)
                bc.painter.drawLine(rect.x, rect.vertCenter(), rect.right(), rect.vertCenter(), pen)
            }

            btn0.sigPressed += mtd|layout|(btn)
            {
                var kind = cast(u64) btn.userData
                .pushTypedValue(cast() kind, FormLine, #nameof(FormLine.beginCap), true)
                .updateBtnState(layout)
            }
        }

        layout.computeLayout()
    }

    mtd createPopupEndCapStyle(btn: *IconButton)
    {
        let frame  = btn.createAnchorPopup(100, @countof(LineCapStyle) * 46.0)
        let layout = StackLayoutCtrl.create(frame, .Top)
        frame.setView(layout)

        mtd updateBtnState(layout: *Wnd)
        {
            let curSel = cast(*FormLine) .getSelectedBaseForm()
            foreach c in layout.childs
            {
                let btn = cast(*IconButton) c
                let cap = cast(LineCapStyle) cast(u64) btn.userData
                btn.isChecked = curSel.endCap == cap
                btn.invalidate()
            }
        }

        let curSel = cast(*FormLine) .getSelectedBaseForm()
        foreach i in LineCapStyle
        {
            let btn0 = IconButton.create(layout, {}, {0, 0, 0, 40})
            btn0.userData = cast(*void) cast(u64) i
            btn0.setForm(.RoundSquare)
            btn0.isChecked = curSel.endCap == i

            btn0.sigPaintIcon = mtd||(btn, bc, pos, flags)
            {
                bc.painter.setParams(.Antialiased)
                var curSel = cast(*FormShape) .getSelectedBaseForm()
                var rect   = pos
                rect.inflate(-9, -9)
                var cap   = cast(LineCapStyle) cast(u64) btn.userData
                let color = flags & .Checked ? btn.getThemeColors().btnIcon_CheckedIcon : btn.getThemeColors().btnIcon_Icon
                var pen   = Pen.createSolid(color, CapSize)
                pen.begCapStyle = .None
                pen.endCapStyle = cap
                var (w0, l0) = Painter.capStyleExtent(cap, CapSize, pen.capEndScaleX)
                rect.moveRight(-l0)
                bc.painter.drawLine(rect.x, rect.vertCenter(), rect.right(), rect.vertCenter(), pen)
            }

            btn0.sigPressed += mtd|layout|(btn)
            {
                var kind = cast(u64) btn.userData
                .pushTypedValue(cast() kind, FormLine, #nameof(FormLine.endCap), true)
                .updateBtnState(layout)
            }
        }

        layout.computeLayout()
    }

    mtd createPopupDashStyle(btn: *IconButton)
    {
        let frame  = btn.createAnchorPopup(180, 170)
        let layout = StackLayoutCtrl.create(frame, .Top)
        frame.setView(layout)

        mtd updateBtnState(layout: *Wnd)
        {
            let curSel = .getSelectedBaseForm()
            foreach c in layout.childs
            {
                let btn  = cast(*IconButton) c
                let dash = cast(DashStyle) cast(u64) btn.userData
                btn.isChecked = curSel.dashStyle == dash
                btn.invalidate()
            }
        }

        let curSel = .getSelectedBaseForm()
        foreach i in DashStyle
        {
            if i == .Pattern do
                continue

            let btn0 = IconButton.create(layout, {}, {0, 0, 0, 32})
            btn0.userData = cast(*void) cast(u64) i
            btn0.setForm(.RoundSquare)
            btn0.isChecked = curSel.dashStyle == i

            btn0.sigPaintIcon = mtd||(btn, bc, pos, flags)
            {
                bc.painter.setParams(.Antialiased)
                var curSel = cast(*FormShape) .getSelectedBaseForm()
                var rect   = pos
                rect.inflate(-9, -9)
                var dash  = cast(DashStyle) cast(u64) btn.userData
                let color = flags & .Checked ? btn.getThemeColors().btnIcon_CheckedIcon : btn.getThemeColors().btnIcon_Icon
                var pen   = Pen.createDash(dash, color, DashSize)
                pen.dashBlankScale = 0.25
                bc.painter.drawLine(rect.x, rect.vertCenter(), rect.right(), rect.vertCenter(), pen)
            }

            btn0.sigPressed += mtd|layout|(btn)
            {
                var kind = cast(u64) btn.userData
                .pushTypedValue(cast() kind, BaseForm, #nameof(BaseForm.dashStyle), true, true)
                .updateBtnState(layout)
            }
        }

        layout.computeLayout()
    }

    mtd propBorderColor(stack: *Wnd)
    {
        let curSel = cast(*FormText) .getSelectedBaseForm()
        if curSel.subKind == .Highlight or curSel.subKind == .PixSel do
            return

        let btn = createArrowButton(stack, BtnColorWidth, BtnColorHeight)

        let curSelType = .getSelectedFormType()
        switch curSelType
        {
        case FormLine, FormArrow, FormDraw:
            btn.name = "Color"
        case FormStep:
            btn.name = "Text"
        default:
            btn.name = "Outline"
        }

        btn.sigPaintIcon = mtd||(btn, bc, pos, flags)
        {
            bc.painter.setParams(.Antialiased)
            let curSel = .getSelectedBaseForm()
            var rect   = pos
            rect.x = rect.horzCenter() - 16
            rect.y = rect.vertCenter() - 16
            rect.width, rect.height = 32
            if !curSel.borderColor.isOpaque() do
                bc.painter.drawRoundRect(rect, 5, 5, Pen.createHatch(.CheckBoardLarge, Argb.DarkGray, 5))
            bc.painter.drawRoundRect(rect, 5, 5, curSel.borderColor, 5)
        }

        btn.sigShowPopup += mtd||(btn)
        {
            let curSel = .getSelectedBaseForm()
            var picker = createColorPicker(btn, curSel.borderColor)
            .pushTypedValue(curSel.borderColor.argb, BaseForm, #nameof(BaseForm.borderColor), true, true)
            picker.sigChanged += mtd|btn|(pk)
            {
                .pushTypedValue(pk.getColor().argb, BaseForm, #nameof(BaseForm.borderColor), false, true)
                btn.invalidate()
            }
        }
    }

    mtd propBackColor(stack: *Wnd)
    {
        let curSel = .getSelectedBaseForm()
        if curSel.subKind != .PixSel
        {
            let curSelType = .getSelectedFormType()
            if curSelType == FormLine or
               curSelType == FormArrow or
               curSelType == FormDraw do
                return

            if curSelType == FormImage
            {
                let imgSel = cast(*FormImage) curSel
                if imgSel.kind != .Image do
                    return
            }
        }

        let btn = createArrowButton(stack, BtnColorWidth, BtnColorHeight)
        btn.name         = "Fill"
        btn.sigPaintIcon = mtd||(btn, bc, pos, flags)
        {
            bc.painter.setParams(.Antialiased)
            var curSel = .getSelectedBaseForm()
            var rect   = pos
            rect.x = rect.horzCenter() - 21
            rect.y = rect.vertCenter() - 21
            rect.width, rect.height = 42
            var backCol = curSel.subKind == .PixSel ? g_Main.pixSelFillCol : curSel.backColor
            if !backCol.isOpaque() do
                bc.painter.fillRoundRect(rect, 10, 10, Brush.createHatch(.CheckBoardLarge, Argb.DarkGray))
            bc.painter.fillRoundRect(rect, 10, 10, backCol)
        }

        btn.sigShowPopup += mtd||(btn)
        {
            var curSel  = .getSelectedBaseForm()
            var backCol = curSel.subKind == .PixSel ? g_Main.pixSelFillCol : curSel.backColor
            var picker  = createColorPicker(btn, backCol)
            if curSel.subKind == .PixSel
            {
                picker.sigChanged += func|btn|(pk)
                {
                    g_Main.pixSelFillCol = pk.getColor()
                    btn.invalidate()
                }
            }
            else
            {
                .pushTypedValue(curSel.backColor.argb, BaseForm, #nameof(BaseForm.backColor), true, true)
                picker.sigChanged += mtd|btn|(pk)
                {
                    .pushTypedValue(pk.getColor().argb, BaseForm, #nameof(BaseForm.backColor), false, true)
                    btn.invalidate()
                }
            }
        }
    }

    mtd propShape(stack: *Wnd)
    {
        let curSelType = .getSelectedFormType()
        if curSelType != FormShape do
            return
        let curSel = .getSelectedBaseForm()
        if curSel.subKind == .PixSel do
            return

        let btn = createArrowButton(stack, BtnColorWidth, BtnColorHeight)
        btn.name         = "Shape"
        btn.sigPaintIcon = mtd||(btn, bc, pos, flags)
        {
            bc.painter.setParams(.Antialiased)
            var curSel = cast(*FormShape) .getSelectedBaseForm()
            var rect   = pos
            rect.inflate(-9, -9)
            rect.x     = rect.horzCenter() - rect.height * 0.5
            rect.width = rect.height
            var color = btn.getThemeColors().btnIcon_Icon

            //#[Swag.Complete]
            switch curSel.kind
            {
            case Ellipse:
                let radius = 16.0
                bc.painter.drawEllipse(pos.horzCenter(), pos.vertCenter(), radius, radius, color, 5)
            case Rectangle:
                bc.painter.drawRect(rect, color, 5)
            case RoundRectangle:
                bc.painter.drawRoundRect(rect, 5, 5, color, 5)
            }
        }

        btn.sigShowPopup += mtd||(btn)
        {
            var curSel = cast(*FormShape) .getSelectedBaseForm()
            .createPopupShapeKind(btn)
        }
    }

    mtd propStep(stack: *Wnd)
    {
        let curSelType = .getSelectedFormType()
        if curSelType != FormStep do
            return

        let btn = createArrowButton(stack, BtnColorWidth, BtnColorHeight)
        btn.name         = "Shape"
        btn.sigPaintIcon = mtd||(btn, bc, pos, flags)
        {
            bc.painter.setParams(.Antialiased)
            var curSel = cast(*FormStep) .getSelectedBaseForm()
            var rect   = pos
            rect.inflate(-9, -9)
            rect.x     = rect.horzCenter() - rect.height * 0.5
            rect.width = rect.height
            var color = btn.getThemeColors().btnIcon_Icon

            #[Swag.Complete]
            switch curSel.kind
            {
            case Ellipse:
                let radius = 16.0
                bc.painter.drawEllipse(pos.horzCenter(), pos.vertCenter(), radius, radius, color, 5)
            case Rectangle:
                bc.painter.drawRect(rect, color, 5)
            case RoundRectangle:
                bc.painter.drawRoundRect(rect, 5, 5, color, 5)
            }
        }

        btn.sigShowPopup += mtd||(btn)
        {
            var curSel = cast(*FormStep) .getSelectedBaseForm()
            .createPopupStepKind(btn)
        }
    }

    mtd propStartCapStyle(stack: *Wnd)
    {
        let curSelType = .getSelectedFormType()
        if curSelType != FormLine and curSelType != FormArrow do
            return

        let btn = createArrowButton(stack, BtnStyleWidth, BtnStyleHeight)
        btn.name = "Start"

        btn.sigPaintIcon = mtd||(btn, bc, pos, flags)
        {
            var curSel = cast(*FormLine) .getSelectedBaseForm()
            var rect   = pos
            rect.inflate(-9, -9)
            var pen = Pen.createSolid(btn.getThemeColors().btnIcon_Icon, CapSize)
            pen.begCapStyle = curSel.beginCap
            pen.endCapStyle = .None
            var (w0, l0) = Painter.capStyleExtent(curSel.beginCap, CapSize, pen.capBegScaleX)
            rect.moveLeft(l0)

            bc.painter.setParams(.Antialiased)
            bc.painter.drawLine(rect.x, rect.vertCenter(), rect.right(), rect.vertCenter(), pen)
        }

        btn.sigShowPopup += mtd||(btn)
        {
            var curSel = cast(*FormShape) .getSelectedBaseForm()
            .createPopupStartCapStyle(btn)
        }
    }

    mtd propEndCapStyle(stack: *Wnd)
    {
        let curSelType = .getSelectedFormType()
        if curSelType != FormLine and curSelType != FormArrow do
            return

        let btn = createArrowButton(stack, BtnStyleWidth, BtnStyleHeight)
        btn.name         = "End"
        btn.sigPaintIcon = mtd||(btn, bc, pos, flags)
        {
            let curSel = cast(*FormLine) .getSelectedBaseForm()
            var rect   = pos
            rect.inflate(-9, -9)
            var pen = Pen.createSolid(btn.getThemeColors().btnIcon_Icon, CapSize)
            pen.begCapStyle = .None
            pen.endCapStyle = curSel.endCap
            let (w0, l0) = Painter.capStyleExtent(curSel.endCap, CapSize, pen.capEndScaleX)
            rect.moveRight(-l0)

            bc.painter.setParams(.Antialiased)
            bc.painter.drawLine(rect.x, rect.vertCenter(), rect.right(), rect.vertCenter(), pen)
        }

        btn.sigShowPopup += mtd||(btn)
        {
            var curSel = cast(*FormShape) .getSelectedBaseForm()
            .createPopupEndCapStyle(btn)
        }
    }

    mtd propDashStyle(stack: *Wnd)
    {
        let curSelType = .getSelectedFormType()
        if curSelType != FormShape and curSelType != FormLine and curSelType != FormArrow and curSelType != FormPolygon do
            return

        let curSel = cast(*FormText) .getSelectedBaseForm()
        if curSel.subKind == .Highlight or curSel.subKind == .PixSel do
            return

        let btn = createArrowButton(stack, BtnStyleWidth, BtnStyleHeight)
        btn.name         = "Dash"
        btn.sigPaintIcon = mtd||(btn, bc, pos, flags)
        {
            bc.painter.setParams(.Antialiased)
            var curSel = .getSelectedBaseForm()
            var rect   = pos
            rect.inflate(-10, -10)
            var pen = Pen.createDash(curSel.dashStyle, btn.getThemeColors().btnIcon_Icon, DashSize)
            pen.dashBlankScale = 0.25
            bc.painter.drawLine(rect.x, rect.vertCenter(), rect.right(), rect.vertCenter(), pen)
        }

        btn.sigShowPopup += mtd||(btn)
        {
            var curSel = cast(*FormShape) .getSelectedBaseForm()
            .createPopupDashStyle(btn)
        }
    }

    mtd propNextStep()
    {
        let curSelType = .getSelectedFormType()
        if curSelType != FormStep do
            return

        let stack = Wnd.create'Wnd(.mainLayout, {0, 0, 0, .getThemeMetrics().comboBox_Height})

        let curSel = cast(*FormStep) .getSelectedBaseForm()
        with let combo = ComboBox.create(stack)
        {
            combo.dockStyle = .Center
            .addItem("1...2...3...")
            .addItem("A...B...C...")
            .addItem("a...b...c...")
            .selectItem(cast(u32) curSel.typeIndex)
            .sigChanged += mtd||(combo, val)
            {
                .pushTypedValue(cast() val, FormStep, #nameof(FormStep.typeIndex), true)
            }
        }

        with let btn = PushButton.create(stack, "Reset", {0, 0, 60})
        {
            .margin.x  = 5
            .dockStyle = .Right
            .toolTip   = "Reset the global step count to 1"
            btn.sigPressed += mtd||(btn)
            {
                let capture = assume .main.getCapture()
                capture.nextStepIndex = 1
            }
        }
    }

    func editText(main: *MainWnd, curText: String)->String
    {
        with let dlg = EditDlg.createMultiLine(main.surface)
        {
            .setMessage("Press Shift+Enter to add a new line")
            .setText(curText)
            .richEdit.selectAll()
            .surface.setTitle("Enter Text")
            .surface.center(main.surface)
            if main.inPlaceEdit do
                .surface.setTopMost()
        }
        if dlg.doModal() != Dialog.BtnOk do
            return curText
        let capture = assume main.getCapture()
        let undo    = capture.newUndo(.SetText)
        undo.text = dlg.getText()
        capture.pushUndo(undo)
        return undo.text
    }

    mtd propText()
    {
        let curSelType = .getSelectedFormType()
        if curSelType != FormText do
            return
        let curSel = cast(*FormText) .getSelectedBaseForm()

        let stack = StackLayoutCtrl.create(.mainLayout, .Top, {0, 0, 0, 160})
        stack.padding = 8

        // Font
        let combo = ComboBox.create(stack)

        combo.addItem(g_App.theme.res.defaultTypeFaceR.getFamilyName())
        if !curSel.typeFace do
            combo.selectedIdx = 0
        combo.addSeparator()

        .sysFonts = FontFamily.enumerateFromOs()
        foreach &fnt in .sysFonts
        {
            combo.addItem(fnt.name, userData0: fnt)
            if curSel.fontName == fnt.name do
                combo.selectedIdx = cast(u32) combo.items.count - 1
        }

        combo.sigChanged += mtd||(combo, idx)
        {
            if idx == Swag.U32.Max do
                return

            var capture = assume .main.getCapture()
            var undo    = capture.newUndo(.SetFont)

            var sel = combo.items[idx]
            if sel.userData0
            {
                let p = cast(*FontFamilyEnumerate) sel.userData0
                undo.text = p.name
            }

            undo.fontStyle = .Regular
            capture.pushUndo(undo)
        }

        // Text
        let edt = EditBox.create(stack, curSel.text)
        edt.rightMargin = 24
        with let btnedt = IconButton.create(edt, me.getTheme().getIcon24(.More, 20), {0, 0, 20, 20})
        {
            .dockStyle = .Right
            .sigPressed += func|edt|(btn)
            {
                let txt = editText(g_Main, edt.text)
                edt.setText(txt)
            }
        }

        edt.name = "Enter text"
        edt.sigLoseFocus += mtd||(edit)
        {
            let curSel = cast(*FormText) .getSelectedBaseForm()
            if curSel.text != edit.text
            {
                let capture = assume .main.getCapture()
                let undo    = capture.newUndo(.SetText)
                undo.text = edit.text
                capture.pushUndo(undo)
            }
        }

        // Bold/Italic
        let stack2 = StackLayoutCtrl.create(stack, .HorzCenter, {0, 0, 0, 30})
        with let bar2 = IconBar.create(stack2, 24)
        {
            .addCheckableToggleItem(Icon.from(&me.main.icons24, 16), CmdIdTextBold)
            .addCheckableToggleItem(Icon.from(&me.main.icons24, 17), CmdIdTextItalic)

            .sigUpdateState += mtd||(evt)
            {
                let curSel = cast(*FormText) .getSelectedBaseForm()
                evt.setFlags.add(.Checked)
                evt.accepted = true

                switch evt.id
                {
                case CmdIdTextBold:
                    evt.checked = curSel.fontStyle == .Bold or curSel.fontStyle == .BoldItalic
                case CmdIdTextItalic:
                    evt.checked = curSel.fontStyle == .Italic or curSel.fontStyle == .BoldItalic
                }
            }

            .sigCheckChanged += mtd||(btn)
            {
                let capture = assume .main.getCapture()
                let curSel  = cast(*FormText) .getSelectedBaseForm()
                var fstyle  = curSel.fontStyle
                switch btn.id
                {
                case CmdIdTextBold:
                    if btn.isChecked and fstyle == .Regular do
                        fstyle = .Bold
                    elif btn.isChecked and fstyle == .Italic do
                        fstyle = .BoldItalic
                    elif !btn.isChecked and fstyle == .Bold do
                        fstyle = .Regular
                    elif !btn.isChecked and fstyle == .BoldItalic do
                        fstyle = .Italic

                case CmdIdTextItalic:
                    if btn.isChecked and fstyle == .Regular do
                        fstyle = .Italic
                    elif btn.isChecked and fstyle == .Bold do
                        fstyle = .BoldItalic
                    elif !btn.isChecked and fstyle == .Italic do
                        fstyle = .Regular
                    elif !btn.isChecked and fstyle == .BoldItalic do
                        fstyle = .Bold
                }

                var undo = capture.newUndo(.SetFontStyle)
                undo.fontStyle = fstyle
                capture.pushUndo(undo)
            }

            bar2.updateCommandState()
        }

        // Align
        let stack1 = StackLayoutCtrl.create(stack, .HorzCenter, {0, 0, 0, 40})
        with let bar = IconBar.create(stack1, 24)
        {
            .addCheckableItem(Icon.from(&me.main.icons24, 8), CmdIdTextHLeft)
            .addCheckableItem(Icon.from(&me.main.icons24, 9), CmdIdTextHCenter)
            .addCheckableItem(Icon.from(&me.main.icons24, 10), CmdIdTextHRight)
            .addCheckableItem(Icon.from(&me.main.icons24, 11), CmdIdTextVTop)
            .addCheckableItem(Icon.from(&me.main.icons24, 12), CmdIdTextVCenter)
            .addCheckableItem(Icon.from(&me.main.icons24, 13), CmdIdTextVBottom)

            .sigUpdateState += mtd||(evt)
            {
                var curSel = cast(*FormText) .getSelectedBaseForm()
                evt.setFlags.add(.Checked)
                evt.accepted = true

                switch evt.id
                {
                case CmdIdTextHLeft:
                    evt.checked = curSel.alignHorz == .LeftTop
                case CmdIdTextHCenter:
                    evt.checked = curSel.alignHorz == .Center
                case CmdIdTextHRight:
                    evt.checked = curSel.alignHorz == .RightBottom
                case CmdIdTextVTop:
                    evt.checked = curSel.alignVert == .LeftTop
                case CmdIdTextVCenter:
                    evt.checked = curSel.alignVert == .Center
                case CmdIdTextVBottom:
                    evt.checked = curSel.alignVert == .RightBottom
                }
            }

            .sigCheckChanged += mtd||(btn)
            {
                var capture = .main.getCapture()
                switch btn.id
                {
                case CmdIdTextHLeft:
                    .pushTypedValue(cast(u32) FormTextAlign.LeftTop, FormText, #nameof(FormText.alignHorz), true)
                case CmdIdTextHCenter:
                    .pushTypedValue(cast(u32) FormTextAlign.Center, FormText, #nameof(FormText.alignHorz), true)
                case CmdIdTextHRight:
                    .pushTypedValue(cast(u32) FormTextAlign.RightBottom, FormText, #nameof(FormText.alignHorz), true)
                case CmdIdTextVTop:
                    .pushTypedValue(cast(u32) FormTextAlign.LeftTop, FormText, #nameof(FormText.alignVert), true)
                case CmdIdTextVCenter:
                    .pushTypedValue(cast(u32) FormTextAlign.Center, FormText, #nameof(FormText.alignVert), true)
                case CmdIdTextVBottom:
                    .pushTypedValue(cast(u32) FormTextAlign.RightBottom, FormText, #nameof(FormText.alignVert), true)
                }
            }

            bar.updateCommandState()
        }
    }

    mtd propStartSize()
    {
        let curSelType = .getSelectedFormType()
        if curSelType != FormLine and curSelType != FormArrow do
            return
        let curSel = cast(*FormLine) .getSelectedBaseForm()

        let ctrl = .createSlider("Start Size", cast() MinCapSize, cast() MaxCapSize, curSel.startSize)
        ctrl.slider.sigStartChange += mtd||(slider)
        {
            .pushTypedValue(cast #bit (u32) slider.getValue(), FormLine, #nameof(FormLine.startSize), true)
        }
        ctrl.slider.sigChanged += mtd||(slider)
        {
            .pushTypedValue(cast #bit (u32) slider.getValue(), FormLine, #nameof(FormLine.startSize), false)
        }
    }

    mtd propEndSize()
    {
        let curSelType = .getSelectedFormType()
        if curSelType != FormLine and curSelType != FormArrow do
            return

        let curSel = cast(*FormLine) .getSelectedBaseForm()
        let ctrl   = .createSlider("End Size", cast() MinCapSize, cast() MaxCapSize, curSel.endSize)
        ctrl.slider.sigStartChange += mtd||(slider)
        {
            .pushTypedValue(cast #bit (u32) slider.getValue(), FormLine, #nameof(FormLine.endSize), true)
        }
        ctrl.slider.sigChanged += mtd||(slider)
        {
            .pushTypedValue(cast #bit (u32) slider.getValue(), FormLine, #nameof(FormLine.endSize), false)
        }
    }

    mtd propRoundSize()
    {
        let curSelType = .getSelectedFormType()
        if curSelType != FormShape do
            return

        let curSel = cast(*FormShape) .getSelectedBaseForm()
        if curSel.kind != .RoundRectangle do
            return

        let ctrl = .createSlider("Round", cast() MinRoundSize, cast() MaxRoundSize, curSel.roundSize)
        ctrl.slider.sigStartChange += mtd||(slider)
        {
            .pushTypedValue(cast #bit (u32) slider.getValue(), FormShape, #nameof(FormShape.roundSize), true)
        }
        ctrl.slider.sigChanged += mtd||(slider)
        {
            .pushTypedValue(cast #bit (u32) slider.getValue(), FormShape, #nameof(FormShape.roundSize), false)
        }
    }

    mtd propXY()
    {
        if !.hasRealSelection() do
            return
        let curSelType = .getSelectedFormType()
        if curSelType != FormShape and curSelType != FormText do
            return
        let curSel = .getSelectedBaseForm()

        {
            let l     = StackLayoutCtrl.create(.mainLayout, .HorzFit, {0, 0, 0, 40})
            let edit0 = EditCtrl.create(l, "x", Format.toString("%", curSel.x), flags: .AutoLoseFocus)
            edit0.labelSize = 15
            edit0.editSize  = 0
            edit0.edit.setInputU64()
            edit0.edit.sigLoseFocus += mtd||(ed)
            {
                let (v, ?, ?) = catch StrConv.parseS32(ed.text)
                .pushTypedValue(v, BaseForm, #nameof(BaseForm.x), true, true)
            }

            let edit1 = EditCtrl.create(l, "y", Format.toString("%", curSel.y), flags: .AutoLoseFocus)
            edit1.labelSize = 15
            edit1.editSize  = 0
            edit1.edit.setInputU64()
            edit1.edit.sigLoseFocus += mtd||(ed)
            {
                let (v, ?, ?) = catch StrConv.parseS32(ed.text)
                .pushTypedValue(v, BaseForm, #nameof(BaseForm.y), true, true)
            }
        }

        {
            let l     = StackLayoutCtrl.create(.mainLayout, .HorzFit, {0, 0, 0, 40})
            let edit0 = EditCtrl.create(l, "w", Format.toString("%", curSel.w), flags: .AutoLoseFocus)
            edit0.labelSize = 15
            edit0.editSize  = 0
            edit0.edit.setInputU64(1, 10000)
            edit0.edit.sigLoseFocus += mtd||(ed)
            {
                let (v, ?, ?) = catch StrConv.parseS32(ed.text)
                .pushTypedValue(v, BaseForm, #nameof(BaseForm.w), true, true)
            }

            let edit1 = EditCtrl.create(l, "h", Format.toString("%", curSel.h), flags: .AutoLoseFocus)
            edit1.labelSize = 15
            edit1.editSize  = 0
            edit1.edit.setInputU64(1, 10000)
            edit1.edit.sigLoseFocus += mtd||(ed)
            {
                let (v, ?, ?) = catch StrConv.parseS32(ed.text)
                .pushTypedValue(v, BaseForm, #nameof(BaseForm.h), true, true)
            }
        }
    }

    mtd propBorderSize()
    {
        let curSelType = .getSelectedFormType()
        if curSelType == FormStep do
            return

        let curSel = .getSelectedBaseForm()
        if curSel.subKind == .PixSel do
            return

        var minS = MinBorderSize
        var maxS = MaxBorderSize
        var labl = "Tickness"
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

        let ctrl = .createSlider(labl, cast() minS, cast() maxS, curSel.borderSize)
        ctrl.slider.sigStartChange += mtd||(slider)
        {
            .pushTypedValue(cast #bit (u32) slider.getValue(), BaseForm, #nameof(BaseForm.borderSize), true, true)
        }
        ctrl.slider.sigChanged += mtd||(slider)
        {
            .pushTypedValue(cast #bit (u32) slider.getValue(), BaseForm, #nameof(BaseForm.borderSize), false, true)
        }
    }

    mtd propFontSize()
    {
        let curSelType = .getSelectedFormType()
        if curSelType != FormText do
            return

        let curSel = cast(*FormText) .getSelectedBaseForm()
        let ctrl   = .createSlider("Size", cast() MinFontSize, cast() MaxFontSize, cast() curSel.fontSize)
        ctrl.slider.sigStartChange += mtd||(slider)
        {
            .pushTypedValue(cast #bit (u32) slider.getValue(), FormText, #nameof(FormText.fontSize), true)
        }
        ctrl.slider.sigChanged += mtd||(slider)
        {
            .pushTypedValue(cast #bit (u32) slider.getValue(), FormText, #nameof(FormText.fontSize), false)
        }
    }

    mtd propScale()
    {
        let curSelType = .getSelectedFormType()
        if curSelType != FormText do
            return

        let curSel = cast(*FormText) .getSelectedBaseForm()
        let ctrl   = .createSlider("Scale", 0, 100, cast() curSel.scale)
        ctrl.edit.unit = "%"

        ctrl.slider.sigStartChange += mtd||(slider)
        {
            .pushTypedValue(cast #bit (u32) slider.getValue(), BaseForm, #nameof(BaseForm.scale), true, true)
        }
        ctrl.slider.sigChanged += mtd||(slider)
        {
            .pushTypedValue(cast #bit (u32) slider.getValue(), BaseForm, #nameof(BaseForm.scale), false, true)
        }
    }

    mtd propRotate()
    {
        let curSelType = .getSelectedFormType()
        if curSelType != FormText and curSelType != FormShape and curSelType != FormImage do
            return
        let curSel = cast(*FormText) .getSelectedBaseForm()
        if curSel.subKind == .Highlight or curSel.subKind == .PixSel do
            return

        let ctrl = .createSlider("Rotate", 0, 360, cast() curSel.rotate)
        ctrl.edit.unit = "°"
        ctrl.slider.sigStartChange += mtd||(slider)
        {
            .pushTypedValue(cast #bit (u32) slider.getValue(), BaseForm, #nameof(BaseForm.rotate), true, true)
        }
        ctrl.slider.sigChanged += mtd||(slider)
        {
            .pushTypedValue(cast #bit (u32) slider.getValue(), BaseForm, #nameof(BaseForm.rotate), false, true)
        }
    }

    mtd propShadow()
    {
        let curSel = .getSelectedBaseForm()
        if curSel.subKind == .PixSel do
            return

        let ctrl = ComboCtrl.create(.mainLayout, "Shadow")
        ctrl.comboSize = 210
        with ctrl.combo
        {
            .addItem("No Shadow")
            .addItem("Small")
            .selectItem(curSel.paintShadow ? 1 : 0)
            .sigChanged += mtd||(combo, val)
            {
                var b = val == 1 ? true : false
                .pushTypedValue(cast() b, BaseForm, #nameof(BaseForm.paintShadow), true, true)
            }
        }
    }

    mtd propSmooth()
    {
        let curSelType = .getSelectedFormType()
        if curSelType != FormLine and curSelType != FormArrow and curSelType != FormPolygon do
            return
        let curSel = cast(*BaseForm) .getSelectedBaseForm()
        if curSel.subKind == .PixSel do
            return

        let ctrl = ComboCtrl.create(.mainLayout, "Line")
        ctrl.comboSize = 210
        with ctrl.combo
        {
            .addItem("Straight")
            .addItem("Bezier")
            .selectItem(curSel.bezier ? 1 : 0)
            .sigChanged += mtd||(combo, val)
            {
                var b = val == 1 ? true : false
                .pushTypedValue(cast() b, BaseForm, #nameof(BaseForm.bezier), true, true)
            }
        }
    }

    mtd propBoldSize()
    {
        let curSelType = .getSelectedFormType()
        if curSelType != FormText do
            return

        let curSel = cast(*FormText) .getSelectedBaseForm()
        let ctrl   = .createSlider("Strengh", cast() MinBoldSize, cast() MaxBoldSize, cast() curSel.boldSize)
        ctrl.slider.sigStartChange += mtd||(slider)
        {
            .pushTypedValue(cast #bit (u32) slider.getValue(), FormText, #nameof(FormText.boldSize), true)
        }
        ctrl.slider.sigChanged += mtd||(slider)
        {
            .pushTypedValue(cast #bit (u32) slider.getValue(), FormText, #nameof(FormText.boldSize), false)
        }
    }

    mtd propOpacity()
    {
        let curSel = .getSelectedBaseForm()
        if curSel.subKind == .PixSel do
            return

        let ctrl = .createSlider("Opacity", 0, 100, cast() curSel.alpha)
        ctrl.edit.unit = "%"

        ctrl.slider.sigStartChange += mtd||(slider)
        {
            .pushTypedValue(cast(u32) slider.getValue(), BaseForm, #nameof(BaseForm.alpha), true, true)
        }
        ctrl.slider.sigChanged += mtd||(slider)
        {
            .pushTypedValue(cast(u32) slider.getValue(), BaseForm, #nameof(BaseForm.alpha), false, true)
        }
    }

    mtd propImageKind()
    {
        let curSelType = .getSelectedFormType()
        if curSelType != FormImage do
            return

        let curSel = cast(*FormImage) .getSelectedBaseForm()

        let ctrl = ComboCtrl.create(.mainLayout, "Kind")
        ctrl.comboSize = 210
        with ctrl.combo
        {
            foreach v in #typeof(FormImageKind).values do
                .addItem(v.name)
            .selectItem(cast(u32) curSel.kind)
            .sigChanged += mtd||(combo, val)
            {
                .pushTypedValue(val, FormImage, #nameof(FormImage.kind), true)
                .selectionHasChanged()
            }
        }

        let ctrl1 = ComboCtrl.create(.mainLayout, "Place")
        ctrl1.comboSize = 210
        with ctrl1.combo
        {
            foreach v in #typeof(FormImagePlacement).values do
                .addItem(v.name)
            .selectItem(cast(u32) curSel.place)
            .sigChanged += mtd||(combo, val)
            {
                .pushTypedValue(val, FormImage, #nameof(FormImage.place), true)
            }
        }
    }

    mtd propStrength()
    {
        let curSelType = .getSelectedFormType()
        if curSelType != FormImage do
            return
        let curSel = cast(*FormImage) .getSelectedBaseForm()
        if curSel.kind == .Image do
            return

        let ctrl = .createSlider("Strength", 0, 100, cast() curSel.strength)
        ctrl.slider.sigStartChange += mtd||(slider)
        {
            .pushTypedValue(cast(u32) slider.getValue(), FormImage, #nameof(FormImage.strength), true)
        }
        ctrl.slider.sigChanged += mtd||(slider)
        {
            .pushTypedValue(cast(u32) slider.getValue(), FormImage, #nameof(FormImage.strength), false)
        }
    }

    mtd propJoinStyle()
    {
        let curSelType = .getSelectedFormType()
        if curSelType != FormText and curSelType != FormPolygon do
            return
        let curSel = .getSelectedBaseForm()
        if curSel.subKind == .PixSel do
            return

        let ctrl = ComboCtrl.create(.mainLayout, "Joins")
        ctrl.comboSize = 210
        with ctrl.combo
        {
            .addItem("Bevel")
            .addItem("Round")
            .addItem("Miter")
            .addItem("MiterBevel")
            .selectItem(cast(u32) curSel.joinStyle - 1)
            .sigChanged += mtd||(combo, val)
            {
                var v = val + 1
                .pushTypedValue(cast() v, BaseForm, #nameof(BaseForm.joinStyle), true, true)
            }
        }
    }

    mtd propPixSel()
    {
        let curSel = .getSelectedBaseForm()
        if curSel.subKind != .PixSel do
            return

        discard PushButton.create(.mainLayout, "", id: ActionPixSel.Fill)
        discard PushButton.create(.mainLayout, "", id: ActionPixSel.SmartFill)
        discard PushButton.create(.mainLayout, "", id: ActionPixSel.Clear)
        discard PushButton.create(.mainLayout, "", id: ActionPixSel.SmartMove)
    }

    mtd propSep()
    {
        let sep = Wnd.create'Wnd(.mainLayout, {0, 0, 0, 1})
        sep.margin.y        = 15
        sep.margin.w        = 8
        sep.backgroundStyle = .Dialog
    }

    mtd selectionHasChanged()
    {
        foreach it in .mainLayout.childs do
            it.destroy()
        if !.getSelectedBaseForm() do
            return

        let stack0 = StackLayoutCtrl.create(.mainLayout, .HorzCenter, {0, 0, 0, BtnColorHeight + 12})
        .propBackColor(stack0)
        .propBorderColor(stack0)
        .propShape(stack0)
        .propStep(stack0)
        if stack0.childs.count == 0 do
            stack0.destroyNow()

        let stack1 = StackLayoutCtrl.create(.mainLayout, .HorzCenter, {0, 0, 0, BtnStyleHeight + 12})
        .propStartCapStyle(stack1)
        .propDashStyle(stack1)
        .propEndCapStyle(stack1)
        if stack1.childs.count == 0 do
            stack1.destroyNow()

        .propImageKind()
        .propStrength()
        .propNextStep()
        .propText()
        .propFontSize()
        .propBoldSize()
        .propBorderSize()
        .propStartSize()
        .propEndSize()
        .propRoundSize()
        .propJoinStyle()
        .propSmooth()
        .propPixSel()

        .propSep()

        .propScale()
        .propRotate()
        .propOpacity()
        .propShadow()

        .propSep()

        .propXY()

        .mainLayout.computeLayout()
        if .mainLayout.childs.count do
            .setScrollSize(0, .mainLayout.childs.back().position.bottom() + 16)
        else do
            .setScrollSize(0, 0)

        let capture = .main.getCapture()
        if capture do
            capture.editView.setFocus()
    }

    func create(parent: *Wnd, main: *MainWnd)->*PropWnd
    {
        with let prop = Wnd.create'PropWnd(parent, {0, 0, 0, 300})
        {
            .backgroundStyle = .Window
            .scrollWndFlags |= .ForceVertical | .SmallBar | .TransparentBar

            .mainLayout            = StackLayoutCtrl.create(prop)
            .mainLayout.layoutKind = .Top
            .mainLayout.padding    = 8
            .mainLayout.padding.y  = 16

            .setView(.mainLayout)

            .main = main
        }

        return prop
    }
}
