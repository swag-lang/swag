using test_foreign

//#[Swag.PrintBc]
#test
{
    var a: Vector2f = {99.0, 105.0}
    var b: Vector2f = {99.0, 100.0}
    var c: Vector2f = {100.0, 99.0}
    var d: Vector2f = {105.0, 99.0}
    var y  = lineLineIntersect1(a, b, c, d)
    @assert(y == 105)
}
