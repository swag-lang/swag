enum FormShapeKind
{
    Ellipse
    Rectangle
    RoundRectangle
}

struct FormShape
{
    using base: BaseForm
    kind:   FormShapeKind = Ellipse

    #[Serialization.NoSerialize]
    {
        gizmo: GizmoRect
    }
}

impl IForm for FormShape
{
    mtd isSame(ptr: *BaseForm)->bool
    {
        other := cast(*FormShape) ptr
        return  borderColor == other.borderColor and
                borderSize  == other.borderSize and
                backColor   == other.backColor and
                alpha       == other.alpha and
                kind        == other.kind and
                dashStyle   == other.dashStyle
    }

    mtd copy(dst, src: *BaseForm)
    {
        dstf := cast(*FormShape) dst
        srcf := cast(*FormShape) src
        dstf.borderColor = srcf.borderColor
        dstf.borderSize  = srcf.borderSize
        dstf.backColor   = srcf.backColor
        dstf.alpha       = srcf.alpha
        dstf.kind        = srcf.kind
        dstf.dashStyle   = srcf.dashStyle
    }

    mtd edit(evt: *MouseEvent, paintRect: Math.Rectangle, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    {
        xa := cast(s32) ((movingStart.x - paintRect.x) / zoom)
        ya := cast(s32) ((movingStart.y - paintRect.y) / zoom)
        xb := cast(s32) ((localPos.x - paintRect.x) / zoom)
        yb := cast(s32) ((localPos.y - paintRect.y) / zoom)
        setBaseForm(xa, ya, xb - xa, yb - ya)
        if evt.kind == .MouseReleased and evt.button == .Left
            return false
        return true
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

    mtd paint(painter: *Painter, z: f32, quickMode: bool)
    {
        bSize := adaptSize(borderSize, MinBorderSize, MaxBorderSize, z, quickMode)
        pp := Math.Rectangle{0, 0, paintPos.width, paintPos.height}
        pen := Pen.createDash(dashStyle, borderColor, bSize)

        #[Swag.Complete]
        switch kind
        {
        case Ellipse:
            sizeX := pp.width * 0.5
            sizeY := pp.height * 0.5
            painter.fillEllipse(pp.x + sizeX, pp.y + sizeY, sizeX, sizeY, backColor)
            if bSize >= 1
                painter.drawEllipse(pp.x + sizeX, pp.y + sizeY, sizeX - bSize * 0.5, sizeY - bSize * 0.5, pen)

        case Rectangle:
            if borderColor.a and (pp.width <= 2*bSize or pp.height < 2*bSize)
                painter.fillRect(pp, borderColor)
            else
            {
                painter.fillRect(pp, backColor)
                rect := pp
                rect.inflate(-bSize * 0.5)
                painter.drawRect(rect, pen)
            }

        case RoundRectangle:
            pp.inflate(-bSize * 0.5)
            round := Math.max(bSize, 10)
            painter.fillRoundRect(pp, round, round, backColor)
            if bSize >= 1
                painter.drawRoundRect(pp, round, round, pen)
        }
    }

    mtd setup(capt: *Capture)
    {
        capture = capt

        gizmo.roundShape = true
        gizmo.anchorSize = 10
        gizmo.trackChange = true
        gizmo.paintSimpleBorder = false
        gizmo.paintBigBorder = true

        gizmo.sigChanged = @|self|(g, first) {
            if !first
                self.capture.undo()
            undo := self.capture.newUndo(.MoveBaseForm)
            undo.baseForm = self
            zoom := self.capture.editView.zoom
            undo.newPosX  = cast(s32) ((g.movingRect.x - g.startRect.x) / zoom)
            undo.newPosY  = cast(s32) ((g.movingRect.y - g.startRect.y) / zoom)
            undo.newSizeX = cast(s32) ((g.movingRect.width - g.startRect.width) / zoom)
            undo.newSizeY = cast(s32) ((g.movingRect.height - g.startRect.height) / zoom)
            self.capture.pushUndo(undo)
            self.capture.editView.invalidate()
        }
    }
}