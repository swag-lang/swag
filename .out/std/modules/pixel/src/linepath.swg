#global public
using Core, Core.Math

enum ElementType
{
    MoveTo
    LineTo
    ArcTo
    CurveTo
    CubicTo
}

struct Element
{
    type:       ElementType = ?
    position:   Vector2
    p1, p2:     Vector2
}

struct LinePath
{
    elements:       Array'Element
    startPoint:     Vector2
    endPoint:       Vector2
    isOpened:       bool
    isClosed:       bool
    isComplex:      bool
}

impl LinePath
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
    func moveTo(using self, pt: Vector2)
    {
        // First point
        if !isOpened
        {
            isOpened = true
            startPoint = pt
        }

        // Its not useful to have two successive moveTo, so replace the last one if
        // necessary
        if elements.count and elements.buffer[elements.count - 1].type == .MoveTo
            elements.buffer[elements.count - 1].position = pt
        else
        {
            var elem: Element = ?
            elem.type = .MoveTo
            elem.position = pt
            elements.add(elem)
        }

        endPoint = pt
    }

    // Add a line from the previous point to 'pt'
    func lineTo(using self, pt: Vector2)
    {
        // Too close from the last point. Just dismiss
        if endPoint.distanceSquared(pt) < 1.0
            return

        var elem: Element = ?
        elem.type = .LineTo
        elem.position = pt
        elements.add(elem)

        endPoint = pt
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