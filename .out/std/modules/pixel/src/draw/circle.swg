#global public
using Core, Drawing, Core.Math

impl Painter
{
    private func setEllipsePath(using self, centerX, centerY: f32, radiusX, radiusY: f32)
    {
        sharedLinePath.clear()
        sharedLinePath.moveTo(centerX, centerY - radiusY)
        sharedLinePath.arcTo(centerX, centerY, radiusX, radiusY, 0, 90)
        sharedLinePath.arcTo(centerX, centerY, radiusX, radiusY, 90, 180)
        sharedLinePath.arcTo(centerX, centerY, radiusX, radiusY, 180, 270)
        sharedLinePath.arcTo(centerX, centerY, radiusX, radiusY, 270, 360)
        sharedLinePath.close()
    }

    func drawCircle(using self, centerX, centerY: f32, radius: f32, color: Color, penSize: f32)
    {
        sharedSolidPen.color = color
        sharedSolidPen.size = penSize
        drawCircle(centerX, centerY, radius, sharedSolidPen)
    }

    func drawCircle(using self, centerX, centerY: f32, radius: f32, pen: Pen)
    {
        setEllipsePath(centerX, centerY, radius, radius)
        drawPath(sharedLinePath, pen)
    }

    func drawEllipse(using self, centerX, centerY: f32, radiusX, radiusY: f32, color: Color, penSize: f32)
    {
        sharedSolidPen.color = color
        sharedSolidPen.size = penSize
        drawEllipse(centerX, centerY, radiusX, radiusY, sharedSolidPen)
    }

    func drawEllipse(using self, centerX, centerY: f32, radiusX, radiusY: f32, pen: Pen)
    {
        setEllipsePath(centerX, centerY, radiusX, radiusY)
        drawPath(sharedLinePath, pen)
    }
}