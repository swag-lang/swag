#global namespace Math
#global public

impl Vector2
{
    // Returns true if the point 'pt' is inside the triangle defined with 'a, b, c'
    func isInTriangle(using const self, a, b, c: Vector2)->bool
    {
        ax  := c.x - b.x
        ay  := c.y - b.y
        bx  := a.x - c.x
        by  := a.y - c.y
        cx  := b.x - a.x
        cy  := b.y - a.y
        apx := x - a.x
        apy := y - a.y
        bpx := x - b.x
        bpy := y - b.y
        cpx := x - c.x
        cpy := y - c.y

        aCROSSbp := ax * bpy - ay * bpx
        cCROSSap := cx * apy - cy * apx
        bCROSScp := bx * cpy - by * cpx

        return (aCROSSbp >= 0) and (bCROSScp >= 0) and (cCROSSap >= 0)
    }

    // Returns the distance of the point to the given segment
    func distanceToLine(using const self, a, b: Vector2)->f32
    {
        toPt := dref self - a
        seg  := b - a
        seg.normalizeSafe()
        dot  := toPt.dot(seg)
        seg  *= dot
        cross := toPt - seg
        return cross.length()
    }
}