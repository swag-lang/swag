using Pixel, Core, Gui

enum FormRectKind
{
    Ellipse
    Rectangle
    RoundRectangle
}

struct FormRect
{
    using base:     BaseForm
    kind:           FormRectKind = Ellipse
    gizmo:          GizmoRect
}

impl IForm for FormRect
{
    mtd moveBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        x += newPosX
        y += newPosY
        w += newSizeX
        h += newSizeY
    }

    mtd moveForm(diff: Math.Point)
    {
        x += cast(s32) diff.x
        y += cast(s32) diff.y
    }

    mtd movePoint(pt: u32, diff: Math.Point)
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

    mtd paint(painter: *Painter, z: f32)
    {
        bSize := borderSize * z
        pp := Math.Rectangle{0, 0, paintPos.width, paintPos.height}

        #[Swag.Complete]
        switch kind
        {
        case Ellipse:
            sizeX := pp.width * 0.5
            sizeY := pp.height * 0.5
            painter.fillEllipse(pp.x + sizeX, pp.y + sizeY, sizeX, sizeY, backColor)
            if bSize >= 1
                painter.drawEllipse(pp.x + sizeX, pp.y + sizeY, sizeX - bSize * 0.5, sizeY - bSize * 0.5, borderColor, bSize)

        case Rectangle:
            if borderColor.a and (pp.width <= 2*bSize or pp.height < 2*bSize)
                painter.fillRect(pp, borderColor)
            else
            {
                painter.fillRect(pp, backColor)
                rect := pp
                rect.inflate(-bSize * 0.5)
                painter.drawRect(rect, borderColor, bSize)
            }

        case RoundRectangle:
            painter.fillRoundRect(pp, 10, 10, backColor)
            if bSize >= 1
            {
                rect := pp
                rect.inflate(-bSize * 0.5)
                painter.drawRoundRect(rect, 10, 10, borderColor, bSize)
            }
        }
    }
}

impl FormRect
{
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