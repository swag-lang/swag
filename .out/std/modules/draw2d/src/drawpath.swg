#global public
using core, mthx

impl DrawingContext
{
    func drawPath(using self, pth: Path, pen: Pen)
    {
        if pen.size == 0
            return
        size2 := pen.size * 0.5

        var vPrevN1: Vector2
        vPrev, vPrevNc       := pth.startPoint
        beginLine, firstLine := true

        visit elem: pth.elements
        {
            if elem.type == .MoveTo
            {
                vPrev, vPrevNc = elem.position
                beginLine = true
                continue
            }

            vNew, vNewNc := elem.position
            n := vNewNc - vPrevNc
            n.normalize()
            n1 := n.cross(1)
            n2 := n1
            n1 *= size2

            var v: [4] Vector2 = ?
            v[0] = vPrev - n1
            v[1] = vNew - n1
            v[3] = vPrev + n1
            v[2] = vNew + n1
            drawSolidTriangle(v[0], v[1], v[3], pen.color)
            drawSolidTriangle(v[1], v[3], v[2], pen.color)

            vPrevN1 = n1
            vPrev = vNew
            vPrevNc = vNewNc
            beginLine, firstLine = false
        }
    }
}