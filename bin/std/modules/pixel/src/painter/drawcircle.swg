#global public
using Core

impl Painter
{
    // Draw an arc between two angles
    #[Swag.Overload]
    mtd drawArc(centerX, centerY: f32, radius: f32, startAngle, endAngle: Math.Angle, color: Color, penSize: f32 = 1)
    {
        if radius == 0 or color.a == 0 do
            return
        if Math.isZeroEpsilon(endAngle.rad - startAngle.rad) do
            return
        Debug.assert(endAngle.rad > startAngle.rad)

        let pen = Pen.createSolid(color, penSize)
        .sharedLinePath.setArc(centerX, centerY, radius, startAngle, endAngle)
        .drawPath(&.sharedLinePath, pen)
    }

    #[Swag.Overload]
    mtd drawArc(centerX, centerY: f32, radius: f32, startAngle, endAngle: Math.Angle, pen: Pen)
    {
        if radius == 0 or pen.brush.color.a == 0 do
            return
        if Math.isZeroEpsilon(endAngle.rad - startAngle.rad) do
            return
        .sharedLinePath.setArc(centerX, centerY, radius, startAngle, endAngle)
        .drawPath(&.sharedLinePath, pen)
    }

    // Draw an empty circle
    #[Swag.Overload]
    mtd drawCircle(centerX, centerY: f32, radius: f32, color: Color, penSize: f32 = 1)
    {
        if radius == 0 or color.a == 0 do
            return

        let pen = Pen.createSolid(color, penSize)
        .drawCircle(centerX, centerY, radius, pen)
    }

    #[Swag.Overload]
    mtd drawCircle(centerX, centerY: f32, radius: f32, pen: Pen)
    {
        .drawEllipse(centerX, centerY, radius, radius, pen)
    }

    // Draw a solid circle
    #[Swag.Overload]
    mtd fillCircle(centerX, centerY: f32, radius: f32, brush: Brush)
    {
        if radius == 0 or brush.color.a == 0 do
            return
        .sharedLinePath.setEllipse(centerX, centerY, radius, radius)
        .fillPath(&.sharedLinePath, brush)
    }

    #[Swag.Overload]
    mtd fillCircle(centerX, centerY: f32, radius: f32, color: Color)
    {
        if radius == 0 or color.a == 0 do
            return

        let brush = Brush.createSolid(color)
        .sharedLinePath.setEllipse(centerX, centerY, radius, radius)
        .fillPath(&.sharedLinePath, brush)
    }

    // Draw an empty ellipse
    #[Swag.Overload]
    mtd drawEllipse(centerX, centerY: f32, radiusX, radiusY: f32, color: Color, penSize: f32 = 1)
    {
        if radiusX == 0 or radiusY == 0 or color.a == 0 or penSize == 0 do
            return

        let pen = Pen.createSolid(color, penSize)
        .drawEllipse(centerX, centerY, radiusX, radiusY, pen)
    }

    #[Swag.Overload]
    mtd drawEllipse(centerX, centerY: f32, radiusX, radiusY: f32, pen: Pen)
    {
        if radiusX == 0 or radiusY == 0 or pen.brush.color.a == 0 do
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

        .sharedLinePath.setEllipseRaw(centerX, centerY, rX, rY)
        .drawPath(&.sharedLinePath, pen)
    }

    #[Swag.Overload]
    mtd drawEllipse(rect: Math.Rectangle, color: Color, penSize: f32 = 1)
    {
        .drawEllipse(rect.horzCenter(), rect.vertCenter(), rect.width * 0.5, rect.height * 0.5, color, penSize)
    }

    #[Swag.Overload]
    mtd drawEllipse(rect: Math.Rectangle, pen: Pen)
    {
        .drawEllipse(rect.horzCenter(), rect.vertCenter(), rect.width * 0.5, rect.height * 0.5, pen)
    }

    // Draw a solid ellipse
    #[Swag.Overload]
    mtd fillEllipse(centerX, centerY: f32, radiusX, radiusY: f32, brush: Brush)
    {
        if radiusX == 0 or radiusY == 0 or brush.color.a == 0 do
            return
        .sharedLinePath.setEllipseRaw(centerX, centerY, radiusX, radiusY)
        .fillPath(&.sharedLinePath, brush)
    }

    #[Swag.Overload]
    mtd fillEllipse(centerX, centerY: f32, radiusX, radiusY: f32, color: Color)
    {
        if radiusX == 0 or radiusY == 0 or color.a == 0 do
            return

        let brush = Brush.createSolid(color)
        .sharedLinePath.setEllipse(centerX, centerY, radiusX, radiusY)
        .fillPath(&.sharedLinePath, brush)
    }

    #[Swag.Overload]
    mtd fillEllipse(rect: Math.Rectangle, brush: Brush)
    {
        .fillEllipse(rect.horzCenter(), rect.vertCenter(), rect.width * 0.5, rect.height * 0.5, brush)
    }

    #[Swag.Overload]
    mtd fillEllipse(rect: Math.Rectangle, color: Color)
    {
        .fillEllipse(rect.horzCenter(), rect.vertCenter(), rect.width * 0.5, rect.height * 0.5, color)
    }
}
