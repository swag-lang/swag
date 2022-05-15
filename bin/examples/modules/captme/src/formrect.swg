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

        #[Swag.Complete]
        switch kind
        {
        case Ellipse:
            sizeX := paintPos.width * 0.5
            sizeY := paintPos.height * 0.5
            painter.fillEllipse(paintPos.x + sizeX, paintPos.y + sizeY, sizeX, sizeY, backColor)
            if bSize >= 1
                painter.drawEllipse(paintPos.x + sizeX, paintPos.y + sizeY, sizeX - bSize * 0.5, sizeY - bSize * 0.5, borderColor, bSize)

        case Rectangle:
            if borderColor.a and (paintPos.width <= 2*bSize or paintPos.height < 2*bSize)
                painter.fillRect(paintPos, borderColor)
            else
            {
                painter.fillRect(paintPos, backColor)
                rect := paintPos
                rect.inflate(-bSize * 0.5)
                painter.drawRect(rect, borderColor, bSize)
            }

        case RoundRectangle:
            painter.fillRoundRect(paintPos, 10, 10, backColor)
            if bSize >= 1
            {
                rect := paintPos
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