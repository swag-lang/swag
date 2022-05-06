using Pixel, Core, Gui

interface IForm
{
    paintGizmo: func(self, *Painter, f32, FormFlags)
    paint:      func(self, *Painter, f32)
    offset:     func(self, s32, s32)
    mouse:      func(self, *EditView, Math.Point, *MouseEvent)->bool
}

enum FormKind
{
    Ellipse
    Rectangle
}

#[Swag.EnumFlags]
enum FormFlags
{
    Zero
    Hot
    Selected
}

struct BaseForm
{
    x, y: s32 = 0
    w, h: s32 = 32

    capture:        *Capture
    paintPos:       Math.Rectangle
    alpha:          f32 = 1
    shadowOffset:   Math.Vector2 = @{2, 2}
}

struct RectShapeForm
{
    using base:     BaseForm
    kind:           FormKind = Ellipse
    borderSize:     f32 = 5
    borderColor:    Color = Argb.White
    backColor:      Color = Argb.Red

    gizmo:          GizmoRect
}

impl IForm for RectShapeForm
{
    mtd mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        return gizmo.mouse(view, pos, evt)
    }

    mtd offset(offX, offY: s32)
    {
        x += offX
        y += offY
    }

    mtd paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    {
        if flags & (.Hot | .Selected)
        {
            gizmo.rect = paintPos
            gizmo.paint(painter, flags & .Selected ? false : true)
        }
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
            painter.fillRect(paintPos, backColor)
            if bSize >= 1
            {
                rect := paintPos
                rect.inflate(-bSize * 0.5)
                painter.drawRect(rect, borderColor, bSize)
            }
        }
    }
}

impl RectShapeForm
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
            undo := Memory.new'Undo()
            undo.kind = .MoveBaseForm
            undo.capture = self.capture
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