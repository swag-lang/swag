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
    using base:     BaseForm
    kind:           FormStepKind = Ellipse
    index:          u32 = 1
    typeIndex       = FormStepType.Number

    #[Serialization.NoSerialize]
    {
        ws, hs:            s32 = 48
        gizmo:             GizmoRect
        isDirty:           bool = true
        cachefSize:        f32
        cacheTypeIndex     = FormStepType.Number
        pathl:             LinePathList
    }
}

impl IForm for FormStep
{
    mtd impl isSame(ptr: *BaseForm)->bool
    {
        let other = cast(*FormStep) ptr
        return .borderColor == other.borderColor and
               .backColor == other.backColor and
               .alpha == other.alpha and
               .kind == other.kind and
               .paintShadow == other.paintShadow and
               .typeIndex == other.typeIndex
    }

    mtd impl copy(dst, src: *BaseForm)
    {
        let dstf = cast(*FormStep) dst
        let srcf = cast(*FormStep) src
        dstf.borderColor = srcf.borderColor
        dstf.backColor   = srcf.backColor
        dstf.alpha       = srcf.alpha
        dstf.kind        = srcf.kind
        dstf.paintShadow = srcf.paintShadow
        dstf.typeIndex   = srcf.typeIndex
        dstf.ws          = srcf.ws
        dstf.hs          = srcf.hs
    }

    mtd impl edit(evt: *MouseEvent, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    {
        var xa = cast(s32) (movingStart.x / zoom)
        var ya = cast(s32) (movingStart.y / zoom)
        xa -= .ws / 2
        ya -= .hs / 2
        .setBaseForm(xa, ya, .ws, .hs)

        var xbu = 0
        var ybu = 0
        .capture.snap(&xbu, &ybu, true, true, 0, cast(IForm) me)
        xa += xbu
        ya += ybu
        .setBaseForm(xa, ya, .ws, .hs)

        .index = .capture.nextStepIndex
        .capture.nextStepIndex += 1
        .isDirty = true

        return false
    }

    mtd impl moveBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        .x += newPosX
        .y += newPosY
        .w += newSizeX
        .h += newSizeY
    }

    mtd impl scaleBaseForm(val: f32)
    {
        .x = cast(s32) (.x * val)
        .y = cast(s32) (.y * val)
        .w = cast(s32) (.w * val)
        .h = cast(s32) (.h * val)
    }

    mtd impl setBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        if newSizeX < 0
        {
            .x = newPosX + newSizeX
            .w = -newSizeX
        }
        else
        {
            .x = newPosX
            .w = newSizeX
        }

        if newSizeY < 0
        {
            .y = newPosY + newSizeY
            .h = -newSizeY
        }
        else
        {
            .y = newPosY
            .h = newSizeY
        }
    }

    mtd impl moveForm(newPosX, newPosY: s32)
    {
        .x += newPosX
        .y += newPosY
    }

    mtd impl movePoint(pt: u32, newPosX, newPosY: s32) {}

    mtd impl boundRect()->Math.Rectangle
    {
        return Math.Rectangle{.x, .y, .w, .h}
    }

    mtd impl mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        return .gizmo.mouse(view, pos, evt)
    }

    mtd impl paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    {
        if flags.has(.Hot | .Selected)
        {
            .gizmo.rect = .boundRect() * z
            .gizmo.paint(painter, !flags.has(.Selected))
        }
    }

    mtd impl isInside(pt: Math.Point)->bool
    {
        return false
    }

    mtd impl hitTest(pt: Math.Point, z: f32)->bool
    {
        return BaseForm.drawHitTest(me, pt, z)
    }

    mtd impl paint(painter: *Painter, cxt: FormPaintContext)
    {
        let z     = cxt.z
        var fSize = Math.min(.w, .h) * 0.6
        fSize = Math.max(fSize, 8)
        let backBrush = Brush.createHatch(.CheckBoardLarge, Argb.DarkGray)

        let face = g_App.theme.res.defaultTypeFaceR

        if fSize != .cachefSize or .typeIndex != .cacheTypeIndex do
            .isDirty = true

        if .isDirty
        {
            .isDirty        = false
            .cachefSize     = fSize
            .cacheTypeIndex = .typeIndex
            .pathl.clear()

            var t:       String
            let curIndex = cxt.quickMode ? 1 : .index

            #[Swag.Complete]
            switch .typeIndex
            {
            case Number:
                t = Format.toString("%", curIndex)
            case LetterUp:
                t = Format.toString("%", cast(rune) ('A''rune + curIndex - 1))
            case LetterDown:
                t = Format.toString("%", cast(rune) ('a''rune + curIndex - 1))
            }

            assume face.getStringOutline(&.pathl, t, cast() fSize)
            .pathl.flatten(painter.curState.paintQuality)
            .pathl.offset(1, JoinStyle.Round, 0.5)
            .pathl.clean()
        }

        var bk = .backColor
        if cxt.quickMode and .alpha != 100 do
            bk.a = cast(u8) Math.map(cast(f32) .alpha, 0, 100, 0, 255)

        let pp = .boundRect() * z

        #[Swag.Complete]
        switch .kind
        {
        case Ellipse:
            let sizeX = pp.width * 0.5
            let sizeY = pp.height * 0.5
            if cxt.quickMode and .alpha != 100 do
                painter.fillEllipse(pp.x + sizeX, pp.y + sizeY, sizeX, sizeY, backBrush)
            painter.fillEllipse(pp.x + sizeX, pp.y + sizeY, sizeX, sizeY, .backColor)

        case Rectangle:
            if cxt.quickMode and .alpha != 100 do
                painter.fillRect(pp, backBrush)
            painter.fillRect(pp, bk)

        case RoundRectangle:
            var round = cast(f32) Math.min(.w, .h)
            round /= 3
            if cxt.quickMode and .alpha != 100 do
                painter.fillRoundRect(pp, round, round, backBrush)
            painter.fillRoundRect(pp, round, round, .backColor)
        }

        let wt = (.pathl.bvMax.x - .pathl.bvMin.x) * z
        let ht = (.pathl.bvMax.y - .pathl.bvMin.y) * z
        let x0 = pp.horzCenter() - (wt * 0.5)
        let y0 = pp.vertCenter() - (ht * 0.5)

        let tt = painter.curState.transform
        with painter
        {
            .pushTransform()
            .curState.transform.tx = 0
            .curState.transform.ty = 0
            .scaleTransform(z, z)
            .translateTransform(tt.tx, tt.ty)
            .translateTransform(x0 - me.pathl.bvMin.x * z, y0 - me.pathl.bvMin.y * z)
            .fillPath(&me.pathl, me.borderColor)
            .popTransform()
        }
    }

    mtd impl setup(capt: *Capture)
    {
        .capture = capt

        with me.gizmo
        {
            .roundShape        = true
            .anchorSize        = 10
            .trackChange       = true
            .paintSimpleBorder = false
            .paintBigBorder    = true
        }

        .gizmo.sigChanged = mtd||(g, first, hitMove)
        {
            g.apply(me, first, hitMove)
        }
    }
}
