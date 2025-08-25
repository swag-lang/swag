#global public
using Core

impl Painter
{
    // Draw an arc between two angles
    #[Swag.Overload]
    mtd drawArc(centerX, centerY: f32, radius: f32, startAngle, endAngle: Math.Angle, color: Color, penSize: f32 = 1)
    {
        if radius == 0 or color.a == 0:
            return
        if Math.isZeroEpsilon(endAngle.rad - startAngle.rad):
            return
        Debug.assert(endAngle.rad > startAngle.rad)

        me.sharedSolidPen.brush.type  = .SolidColor
        me.sharedSolidPen.brush.color = color
        me.sharedSolidPen.size        = penSize

        me.sharedLinePath.setArc(centerX, centerY, radius, startAngle, endAngle)
        me.drawPath(&me.sharedLinePath, me.sharedSolidPen)
    }

    #[Swag.Overload]
    mtd drawArc(centerX, centerY: f32, radius: f32, startAngle, endAngle: Math.Angle, pen: Pen)
    {
        if radius == 0 or pen.brush.color.a == 0:
            return
        if Math.isZeroEpsilon(endAngle.rad - startAngle.rad):
            return
        me.sharedLinePath.setArc(centerX, centerY, radius, startAngle, endAngle)
        me.drawPath(&me.sharedLinePath, pen)
    }

    // Draw an empty circle
    #[Swag.Overload]
    mtd drawCircle(centerX, centerY: f32, radius: f32, color: Color, penSize: f32 = 1)
    {
        if radius == 0 or color.a == 0:
            return
        me.sharedSolidPen.brush.type  = .SolidColor
        me.sharedSolidPen.brush.color = color
        me.sharedSolidPen.size        = penSize
        me.drawCircle(centerX, centerY, radius, me.sharedSolidPen)
    }

    #[Swag.Overload]
    mtd drawCircle(centerX, centerY: f32, radius: f32, pen: Pen)
    {
        me.drawEllipse(centerX, centerY, radius, radius, pen)
    }

    // Draw a solid circle
    #[Swag.Overload]
    mtd fillCircle(centerX, centerY: f32, radius: f32, brush: Brush)
    {
        if radius == 0 or brush.color.a == 0:
            return
        me.sharedLinePath.setEllipse(centerX, centerY, radius, radius)
        me.fillPath(&me.sharedLinePath, brush)
    }

    #[Swag.Overload]
    mtd fillCircle(centerX, centerY: f32, radius: f32, color: Color)
    {
        if radius == 0 or color.a == 0:
            return
        me.sharedSolidBrush.type  = .SolidColor
        me.sharedSolidBrush.color = color
        me.sharedLinePath.setEllipse(centerX, centerY, radius, radius)
        me.fillPath(&me.sharedLinePath, me.sharedSolidBrush)
    }

    // Draw an empty ellipse
    #[Swag.Overload]
    mtd drawEllipse(centerX, centerY: f32, radiusX, radiusY: f32, color: Color, penSize: f32 = 1)
    {
        if radiusX == 0 or radiusY == 0 or color.a == 0 or penSize == 0:
            return
        me.sharedSolidPen.brush.type  = .SolidColor
        me.sharedSolidPen.brush.color = color
        me.sharedSolidPen.size        = penSize
        me.drawEllipse(centerX, centerY, radiusX, radiusY, me.sharedSolidPen)
    }

    #[Swag.Overload]
    mtd drawEllipse(centerX, centerY: f32, radiusX, radiusY: f32, pen: Pen)
    {
        if radiusX == 0 or radiusY == 0 or pen.brush.color.a == 0:
            return

        var rX = radiusX
        var rY = radiusY

        #[Swag.Complete]
        switch pen.borderPos
        {
        case None:
            break
        case Inside:
            rX -= pen.size * 0.5
            rY -= pen.size * 0.5
        }

        me.sharedLinePath.setEllipseRaw(centerX, centerY, rX, rY)
        me.drawPath(&me.sharedLinePath, pen)
    }

    #[Swag.Overload]
    mtd drawEllipse(rect: Math.Rectangle, color: Color, penSize: f32 = 1)
    {
        me.drawEllipse(rect.horzCenter(), rect.vertCenter(), rect.width * 0.5, rect.height * 0.5, color, penSize)
    }

    #[Swag.Overload]
    mtd drawEllipse(rect: Math.Rectangle, pen: Pen)
    {
        me.drawEllipse(rect.horzCenter(), rect.vertCenter(), rect.width * 0.5, rect.height * 0.5, pen)
    }

    // Draw a solid ellipse
    #[Swag.Overload]
    mtd fillEllipse(centerX, centerY: f32, radiusX, radiusY: f32, brush: Brush)
    {
        if radiusX == 0 or radiusY == 0 or brush.color.a == 0:
            return
        me.sharedLinePath.setEllipseRaw(centerX, centerY, radiusX, radiusY)
        me.fillPath(&me.sharedLinePath, brush)
    }

    #[Swag.Overload]
    mtd fillEllipse(centerX, centerY: f32, radiusX, radiusY: f32, color: Color)
    {
        if radiusX == 0 or radiusY == 0 or color.a == 0:
            return
        me.sharedSolidBrush.type  = .SolidColor
        me.sharedSolidBrush.color = color
        me.sharedLinePath.setEllipse(centerX, centerY, radiusX, radiusY)
        me.fillPath(&me.sharedLinePath, me.sharedSolidBrush)
    }

    #[Swag.Overload]
    mtd fillEllipse(rect: Math.Rectangle, brush: Brush)
    {
        me.fillEllipse(rect.horzCenter(), rect.vertCenter(), rect.width * 0.5, rect.height * 0.5, brush)
    }

    #[Swag.Overload]
    mtd fillEllipse(rect: Math.Rectangle, color: Color)
    {
        me.fillEllipse(rect.horzCenter(), rect.vertCenter(), rect.width * 0.5, rect.height * 0.5, color)
    }
}
