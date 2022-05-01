using Pixel, Core

interface IForm
{
    paint:  func(self, *Painter, f32, f32, f32, FormFlags)
    offset: func(self, s32, s32)
}

enum FormKind
{
    Ellipse
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

    paintPos: Math.Rectangle
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
    mtd offset(offX, offY: s32)
    {
        x += offX
        y += offY
    }

    mtd paint(painter: *Painter, refX, refY, z: f32, flags: FormFlags)
    {
        paintPos.x      = refX + (x * z)
        paintPos.y      = refY + (y * z)
        paintPos.width  = w * z
        paintPos.height = h * z

        bSize := borderSize * z

        if flags & (.Hot | .Selected)
        {
            gizmo.rect = paintPos
            gizmo.paintBorder = false
            gizmo.roundShape = true
            gizmo.anchorSize = 10
            gizmo.paint(painter, flags & .Selected ? 1.0 : 0.8)
        }

        #[Swag.Complete]
        switch kind
        {
        case Ellipse:
            sizeX := paintPos.width * 0.5
            sizeY := paintPos.height * 0.5
            painter.fillEllipse(paintPos.x + sizeX, paintPos.y + sizeY, sizeX, sizeY, backColor)
            if bSize >= 1
                painter.drawEllipse(paintPos.x + sizeX, paintPos.y + sizeY, sizeX - bSize * 0.5, sizeY - bSize * 0.5, borderColor, bSize)
        }
    }
}