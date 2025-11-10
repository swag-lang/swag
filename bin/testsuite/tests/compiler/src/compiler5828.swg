struct Vector2f { x, y: f32 }

func getLineLineIntersectCoefs1(start0, end0, start1, end1: Vector2f)->{ d0: f32, d1: f32 }
{
    let ua = (end1.x - start1.x) * (start0.y - start1.y)
    let ub = (end0.x - start0.x) * (start0.y - start1.y)
    let d  = (end1.y - start1.y) * (end0.x - start0.x) + (end1.x - start1.x) * (end0.y - start0.y)
    return {ua / d, ub / d}
}

//#[Swag.PrintAsm]
//#[Swag.Optimize("backend", false)]
func lineLineIntersect1(start0, end0, start1, end1: Vector2f)->f32
{
    let aa = getLineLineIntersectCoefs1(start0, end0, start1, end1)
    return start0.y
}

//#[Swag.PrintBc]
#test
{
    let a: Vector2f = {99.0, 105.0}
    let b: Vector2f = {99.0, 100.0}
    let c: Vector2f = {100.0, 99.0}
    let d: Vector2f = {105.0, 99.0}
    let y  = lineLineIntersect1(a, b, c, d)
    @assert(y == 105)
}
