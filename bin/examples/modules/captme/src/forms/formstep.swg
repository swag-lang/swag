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
        return me.borderColor == other.borderColor and
               me.backColor == other.backColor and
               me.alpha == other.alpha and
               me.kind == other.kind and
               me.paintShadow == other.paintShadow and
               me.typeIndex == other.typeIndex
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
        xa -= me.ws / 2
        ya -= me.hs / 2
        me.setBaseForm(xa, ya, me.ws, me.hs)

        var xbu = 0
        var ybu = 0
        me.capture.snap(&xbu, &ybu, true, true, 0, cast(IForm) me)
        xa += xbu
        ya += ybu
        me.setBaseForm(xa, ya, me.ws, me.hs)

        me.index = me.capture.nextStepIndex
        me.capture.nextStepIndex += 1
        me.isDirty = true

        return false
    }

    mtd impl moveBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        me.x += newPosX
        me.y += newPosY
        me.w += newSizeX
        me.h += newSizeY
    }

    mtd impl scaleBaseForm(val: f32)
    {
        me.x = cast(s32) (me.x * val)
        me.y = cast(s32) (me.y * val)
        me.w = cast(s32) (me.w * val)
        me.h = cast(s32) (me.h * val)
    }

    mtd impl setBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        if newSizeX < 0
        {
            me.x = newPosX + newSizeX
            me.w = -newSizeX
        }
        else
        {
            me.x = newPosX
            me.w = newSizeX
        }

        if newSizeY < 0
        {
            me.y = newPosY + newSizeY
            me.h = -newSizeY
        }
        else
        {
            me.y = newPosY
            me.h = newSizeY
        }
    }

    mtd impl moveForm(newPosX, newPosY: s32)
    {
        me.x += newPosX
        me.y += newPosY
    }

    mtd impl movePoint(pt: u32, newPosX, newPosY: s32) {}

    mtd impl boundRect()->Math.Rectangle
    {
        return Math.Rectangle{me.x, me.y, me.w, me.h}
    }

    mtd impl mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        return me.gizmo.mouse(view, pos, evt)
    }

    mtd impl paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    {
        if flags.has(.Hot | .Selected)
        {
            me.gizmo.rect = me.boundRect() * z
            me.gizmo.paint(painter, !flags.has(.Selected))
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
        var fSize = Math.min(me.w, me.h) * 0.6
        fSize = Math.max(fSize, 8)
        let backBrush = Brush.createHatch(.CheckBoardLarge, Argb.DarkGray)

        let face = g_App.theme.res.defaultTypeFaceR

        if fSize != me.cachefSize or me.typeIndex != me.cacheTypeIndex:
            me.isDirty = true

        if me.isDirty
        {
            me.isDirty        = false
            me.cachefSize     = fSize
            me.cacheTypeIndex = me.typeIndex
            me.pathl.clear()

            var t:       String
            let curIndex = cxt.quickMode ? 1 : me.index

            #[Swag.Complete]
            switch me.typeIndex
            {
            case Number:
                t = Format.toString("%", curIndex)
            case LetterUp:
                t = Format.toString("%", cast(rune) ('A''rune + curIndex - 1))
            case LetterDown:
                t = Format.toString("%", cast(rune) ('a''rune + curIndex - 1))
            }

            assume face.getStringOutline(&me.pathl, t, cast() fSize)
            me.pathl.flatten(painter.curState.paintQuality)
            me.pathl.offset(1, JoinStyle.Round, 0.5)
            me.pathl.clean()
        }

        var bk = me.backColor
        if cxt.quickMode and me.alpha != 100:
            bk.a = cast(u8) Math.map(cast(f32) me.alpha, 0, 100, 0, 255)

        let pp = me.boundRect() * z

        #[Swag.Complete]
        switch me.kind
        {
        case Ellipse:
            let sizeX = pp.width * 0.5
            let sizeY = pp.height * 0.5
            if cxt.quickMode and me.alpha != 100:
                painter.fillEllipse(pp.x + sizeX, pp.y + sizeY, sizeX, sizeY, backBrush)
            painter.fillEllipse(pp.x + sizeX, pp.y + sizeY, sizeX, sizeY, me.backColor)

        case Rectangle:
            if cxt.quickMode and me.alpha != 100:
                painter.fillRect(pp, backBrush)
            painter.fillRect(pp, bk)

        case RoundRectangle:
            var round = cast(f32) Math.min(me.w, me.h)
            round /= 3
            if cxt.quickMode and me.alpha != 100:
                painter.fillRoundRect(pp, round, round, backBrush)
            painter.fillRoundRect(pp, round, round, me.backColor)
        }

        let wt = (me.pathl.bvMax.x - me.pathl.bvMin.x) * z
        let ht = (me.pathl.bvMax.y - me.pathl.bvMin.y) * z
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
        me.capture = capt

        with me.gizmo
        {
            .roundShape        = true
            .anchorSize        = 10
            .trackChange       = true
            .paintSimpleBorder = false
            .paintBigBorder    = true
        }

        me.gizmo.sigChanged = mtd||(g, first, hitMove)
        {
            g.apply(me, first, hitMove)
        }
    }
}
