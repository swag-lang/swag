#global public
using Core, Drawing, Core.Math

impl Painter
{
    // Draw an empty circle
    func drawCircle(using self, centerX, centerY: f32, radius: f32, color: Color, penSize: f32)
    {
        sharedSolidPen.brush.type = .SolidColor
        sharedSolidPen.brush.color = color
        sharedSolidPen.size = penSize
        drawCircle(centerX, centerY, radius, sharedSolidPen)
    }

    // Draw an empty circle
    func drawCircle(using self, centerX, centerY: f32, radius: f32, pen: Pen)
    {
        sharedLinePath.setEllipse(centerX, centerY, radius, radius)
        drawPath(&sharedLinePath, pen)
    }

    // Draw a solid circle
    func fillCircle(using self, centerX, centerY: f32, radius: f32, brush: Brush)
    {
        sharedLinePath.setEllipse(centerX, centerY, radius, radius)
        fillPath(&sharedLinePath, brush)
    }

    // Draw a solid circle
    func fillCircle(using self, centerX, centerY: f32, radius: f32, color: Color)
    {
        sharedSolidBrush.type = .SolidColor
        sharedSolidBrush.color = color
        sharedLinePath.setEllipse(centerX, centerY, radius, radius)
        fillPath(&sharedLinePath, sharedSolidBrush)
    }

    // Draw an empty ellipse
    func drawEllipse(using self, centerX, centerY: f32, radiusX, radiusY: f32, color: Color, penSize: f32)
    {
        sharedSolidPen.brush.type = .SolidColor
        sharedSolidPen.brush.color = color
        sharedSolidPen.size = penSize
        drawEllipse(centerX, centerY, radiusX, radiusY, sharedSolidPen)
    }

    // Draw an empty ellipse
    func drawEllipse(using self, centerX, centerY: f32, radiusX, radiusY: f32, pen: Pen)
    {
        sharedLinePath.setEllipse(centerX, centerY, radiusX, radiusY)
        drawPath(&sharedLinePath, pen)
    }

    // Draw a solid ellipse
    func fillEllipse(using self, centerX, centerY: f32, radiusX, radiusY: f32, brush: Brush)
    {
        sharedLinePath.setEllipse(centerX, centerY, radiusX, radiusY)
        fillPath(&sharedLinePath, brush)
    }

    // Draw a solid ellipse
    func fillEllipse(using self, centerX, centerY: f32, radiusX, radiusY: f32, color: Color)
    {
        sharedSolidBrush.type = .SolidColor
        sharedSolidBrush.color = color
        sharedLinePath.setEllipse(centerX, centerY, radiusX, radiusY)
        fillPath(&sharedLinePath, sharedSolidBrush)
    }
}