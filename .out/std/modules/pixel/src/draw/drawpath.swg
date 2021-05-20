#global public
using Core, Core.Math

impl Painter
{
    func drawPath(using self, pth: LinePath, pen: Pen)
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

            v0 := vPrev - n1
            v1 := vNew - n1
            v3 := vPrev + n1
            v2 := vNew + n1
            drawSolidTriangle(v0, v1, v3, pen.color)
            drawSolidTriangle(v1, v3, v2, pen.color)

            vPrevN1 = n1
            vPrev = vNew
            vPrevNc = vNewNc
            beginLine, firstLine = false
        }
    }
}