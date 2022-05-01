using Pixel, Core

interface IForm
{
    paint:  func(self, *Painter, f32, f32, f32)
}

enum FormKind
{
    Ellipse
}

struct ShapeForm
{
    kind:           FormKind = Ellipse
    x, y:           f32 = 0
    w, h:           f32 = 32
    borderSize:     f32 = 5
    borderColor:    Color = Argb.White
    backColor:      Color = Argb.Red

    paintPos:       Math.Rectangle
}

impl IForm for ShapeForm
{
    mtd paint(painter: *Painter, refX, refY, z: f32)
    {
        paintPos.x      = refX + (x * z)
        paintPos.y      = refY + (y * z)
        paintPos.width  = w * z
        paintPos.height = h * z

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
        }
    }
}