#global namespace Math
#global public

func isVec2InTriangle(pt: Vector2, a, b, c: Vector2)->bool
{
    ax  := c.x - b.x
    ay  := c.y - b.y
    bx  := a.x - c.x
    by  := a.y - c.y
    cx  := b.x - a.x
    cy  := b.y - a.y
    apx := pt.x - a.x
    apy := pt.y - a.y
    bpx := pt.x - b.x
    bpy := pt.y - b.y
    cpx := pt.x - c.x
    cpy := pt.y - c.y

    aCROSSbp := ax * bpy - ay * bpx
    cCROSSap := cx * apy - cy * apx
    bCROSScp := bx * cpy - by * cpx

    return (aCROSSbp >= 0) and (bCROSScp >= 0) and (cCROSSap >= 0)
}