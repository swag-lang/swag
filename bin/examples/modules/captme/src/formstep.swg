enum FormStepKind
{
    Ellipse
    Rectangle
    RoundRectangle
}

enum FormStepType
{
    Number
    LetterUp
    LetterDown
}

struct FormStep
{
    using base: BaseForm
    kind:     FormStepKind = Ellipse
    index:    u32 = 1
    typeIndex = FormStepType.Number

    #[Serialization.NoSerialize]
    {
        ws, hs:         s32 = 48
        gizmo:          GizmoRect
        isDirty:        bool
        cachefSize:     f32
        cacheTypeIndex  = FormStepType.Number
        pathl:          LinePathList
    }
}

impl IForm for FormStep
{
    mtd isSame(ptr: *BaseForm)->bool
    {
        other := cast(*FormStep) ptr
        return  borderColor == other.borderColor and
                backColor   == other.backColor and
                alpha       == other.alpha and
                kind        == other.kind and
                paintShadow == other.paintShadow and
                typeIndex   == other.typeIndex
    }

    mtd copy(dst, src: *BaseForm)
    {
        dstf := cast(*FormStep) dst
        srcf := cast(*FormStep) src
        dstf.borderColor = srcf.borderColor
        dstf.backColor   = srcf.backColor
        dstf.alpha       = srcf.alpha
        dstf.kind        = srcf.kind
        dstf.paintShadow = srcf.paintShadow
        dstf.typeIndex   = srcf.typeIndex
        dstf.ws          = srcf.ws
        dstf.hs          = srcf.hs
    }

    mtd edit(evt: *MouseEvent, paintRect: Math.Rectangle, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    {
        xa := cast(s32) ((movingStart.x - paintRect.x) / zoom)
        ya := cast(s32) ((movingStart.y - paintRect.y) / zoom)
        xa -= ws/2
        ya -= hs/2
        setBaseForm(xa, ya, ws, hs)

        xbu := 0
        ybu := 0
        capture.snap(&xbu, &ybu, true, true, 0, cast(IForm) self)
        xa += xbu
        ya += ybu
        setBaseForm(xa, ya, ws, hs)

        index = capture.nextStepIndex
        capture.nextStepIndex += 1
        isDirty = true

        return false
    }

    mtd moveBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        x += newPosX
        y += newPosY
        w += newSizeX
        h += newSizeY
    }

    mtd setBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        if newSizeX < 0
        {
            x = newPosX + newSizeX
            w = -newSizeX
        }
        else
        {
            x = newPosX
            w = newSizeX
        }

        if newSizeY < 0
        {
            y = newPosY + newSizeY
            h = -newSizeY
        }
        else
        {
            y = newPosY
            h = newSizeY
        }
    }

    mtd moveForm(newPosX, newPosY: s32)
    {
        x += newPosX
        y += newPosY
    }

    mtd movePoint(pt: u32, newPosX, newPosY: s32)
    {
    }

    mtd boundRect()->Math.Rectangle
    {
        return Math.Rectangle{x, y, w, h}
    }

    mtd mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        return gizmo.mouse(view, pos, evt)
    }

    mtd paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    {
        if flags & (.Hot | .Selected)
        {
            gizmo.rect = paintPos
            gizmo.paint(painter, flags & .Selected ? false : true)
        }
    }

    mtd isInside(pt: Math.Point)->bool
    {
        return false
    }

    mtd hitTest(pt: Math.Point, z: f32)->bool
    {
        switch kind
        {
        case Ellipse:
            pos := Math.Vector2{pt.x, pt.y}
            cen := Math.Vector2{paintPos.horzCenter(), paintPos.vertCenter()}
            if !pos.isInEllipse(cen, paintPos.width * 0.5, paintPos.height * 0.5)
                return false
            if backColor.a == 0 and pos.isInEllipse(cen, (paintPos.width * 0.5) - (2*borderSize), (paintPos.height * 0.5) - (2*borderSize))
                return false

        case Rectangle, RoundRectangle:
            rect := paintPos
            rect.inflate(-2*borderSize)
            if backColor.a == 0 and rect.contains(pt)
                return false
        }

        return true
    }

    mtd paint(app: *Application, painter: *Painter, z: f32, quickMode: bool)
    {
        pp        := Math.Rectangle{0, 0, paintPos.width, paintPos.height}
        fSize     := Math.min(w, h) * 0.6
        fSize     = Math.max(fSize, 8)
        backBrush := Brush.createHatch(.CheckBoardLarge, Argb.DarkGray)

        face := g_App.theme.res.defaultTypeFaceR

        if fSize != cachefSize or typeIndex != cacheTypeIndex
            isDirty = true

        if isDirty
        {
            isDirty = false
            cachefSize = fSize
            cacheTypeIndex = typeIndex
            pathl.clear()

            var t: String
            curIndex := quickMode ? 1 : index

            #[Swag.Complete]
            switch typeIndex
            {
            case Number:     t = Format.toString("%", curIndex)
            case LetterUp:   t = Format.toString("%", cast(rune) ("A"'rune + curIndex - 1))
            case LetterDown: t = Format.toString("%", cast(rune) ("a"'rune + curIndex - 1))
            }

            assume face.getStringOutline(&pathl, t, acast fSize)
            pathl.flatten(painter.curState.paintQuality)
            pathl.offset(1, JoinStyle.Round, 0.5)
            pathl.clean()
        }

        bk := backColor
        if quickMode and alpha != 100
            bk.a = cast(u8) Math.map(cast(f32) alpha, 0, 100, 0, 255)

        #[Swag.Complete]
        switch kind
        {
        case Ellipse:
            sizeX := pp.width * 0.5
            sizeY := pp.height * 0.5
            if quickMode and alpha != 100
                painter.fillEllipse(pp.x + sizeX, pp.y + sizeY, sizeX, sizeY, backBrush)
            painter.fillEllipse(pp.x + sizeX, pp.y + sizeY, sizeX, sizeY, backColor)

        case Rectangle:
            if quickMode and alpha != 100
                painter.fillRect(pp, backBrush)
            painter.fillRect(pp, bk)

        case RoundRectangle:
            round := cast(f32) (Math.min(w, h))
            round /= 3
            if quickMode and alpha != 100
                painter.fillRoundRect(pp, round, round, backBrush)
            painter.fillRoundRect(pp, round, round, backColor)
        }

        wt := (pathl.bvMax.x - pathl.bvMin.x) * z
        ht := (pathl.bvMax.y - pathl.bvMin.y) * z
        x0 := pp.horzCenter() - (wt * 0.5)
        y0 := pp.vertCenter() - (ht * 0.5)
        painter.pushState()
        painter.scaleTransform(z, z)
        painter.translateTransform(x0 - (pathl.bvMin.x * z), y0 - (pathl.bvMin.y * z))
        painter.fillPath(&pathl, borderColor)
        painter.popState()
    }

    mtd setup(capt: *Capture)
    {
        capture = capt

        with gizmo
        {
            .roundShape = true
            .anchorSize = 10
            .trackChange = true
            .paintSimpleBorder = false
            .paintBigBorder = true
        }

        gizmo.sigChanged = closure|self|(g, first, hitMove) {
            g.apply(self, first, hitMove)
        }
    }
}