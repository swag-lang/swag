using Core, Core.Math

enum ElementType
{
    Move
    Line
    Arc
    Bezier1
    Bezier2
}

struct Element
{
    type:       ElementType = ?
    position:   Vector2
    p1, p2:     Vector2
}

#[Swag.opaque]
public struct LinePath
{
    elements:       Array'Element
    startPoint:     Vector2
    endPoint:       Vector2
    isOpened:       bool
    isClosed:       bool
    isComplex:      bool
}

public impl LinePath
{
    // Reset the path
    func clear(using self)
    {
        elements.clear()
        isOpened, isClosed, isComplex = false
        startPoint = Vector2.Zero
        endPoint = Vector2.Zero
    }

    // Move the current cursor position
    #[Swag.inline]
    func moveTo(using self, pt: Vector2)
    {
        moveTo(pt.x, pt.y)
    }

    func moveTo(using self, x, y: f32)
    {
        // First point
        if !elements.count
        {
            startPoint.set(x, y)
            endPoint.set(x, y)
            return
        }

        // Its not useful to have two successive moveTo, so replace the last one if
        // necessary
        if elements.buffer[elements.count - 1].type == .Move
        {
            elements.buffer[elements.count - 1].position.set(x, y)
            endPoint.set(x, y)
            return
        }

        var elem: Element = ?
        elem.type = .Move
        elem.position.set(x, y)
        elements.add(elem)
        endPoint.set(x, y)
    }

    // Add a line from the previous point to 'pt'
    #[Swag.inline]
    func lineTo(using self, pt: Vector2)
    {
        lineTo(pt.x, pt.y)
    }

    func lineTo(using self, x, y: f32)
    {
        var elem: Element = ?
        elem.type = .Line
        elem.position.set(x, y)
        elements.add(elem)
        endPoint.set(x, y)
    }

    // Add a curve from the previous point to 'pt'
    #[Swag.inline]
    func bezierTo(using self, pt, ct: Vector2)
    {
        bezierTo(pt.x, pt.y, ct.x, ct.y)
    }

    func bezierTo(using self, x, y: f32, ctx, cty: f32)
    {
        var elem: Element = ?
        elem.type = .Bezier1
        elem.position.set(x, y)
        elem.p1.set(ctx, cty)
        elements.add(elem)
        endPoint.set(x, y)
    }

    // Add a curve from the previous point to 'pt'
    #[Swag.inline]
    func bezierTo(using self, pt, ct1, ct2: Vector2)
    {
        bezierTo(pt.x, pt.y, ct1.x, ct1.y, ct2.x, ct2.y)
    }

    func bezierTo(using self, x, y: f32, ct1x, ct1y: f32, ct2x, ct2y: f32)
    {
        var elem: Element = ?
        elem.type = .Bezier2
        elem.position.set(x, y)
        elem.p1.set(ct1x, ct1y)
        elem.p2.set(ct2x, ct2y)
        elements.add(elem)
        endPoint.set(x, y)
    }

    // Add an arc from the previous point
    #[Swag.inline]
    func arcTo(using self, center: Vector2, radiusX, radiusY: f32, initAngle, maxAngle: f32)
    {
        arcTo(center.x, center.y, radiusX, radiusY, initAngle, maxAngle)
    }

    func arcTo(using self, centerX, centerY: f32, radiusX, radiusY: f32, initAngle, maxAngle: f32)
    {
        initAngleRad := Math.toRadians(initAngle)
        maxAngleRad := Math.toRadians(maxAngle)

        var elem: Element = ?
        elem.type = .Arc
        elem.position.set(centerX, centerY)
        elem.p1.x = initAngleRad
        elem.p1.y = maxAngleRad
        elem.p2.x = radiusX
        elem.p2.y = radiusY
        elements.add(elem)

        endPoint.x = centerX + (Math.sin(maxAngleRad) * radiusX)
        endPoint.y = centerY + (-Math.cos(maxAngleRad) * radiusY)
    }

    // Close the figure
    // Will add a line to the first point if necessary
    func close(using self)
    {
        if isClosed return

        v := endPoint - startPoint
        if v.lengthSquared() >= 0.5 * 0.5
            lineTo(startPoint)

        isClosed = true
    }
}