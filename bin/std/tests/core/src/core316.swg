#global skip
using Core, Core.Math

#test
{
    var a: Vector2 = {99.0, 105.0}
    var b: Vector2 = {99.0, 100.0}
    var c: Vector2 = {100.0, 99.0}
    var d: Vector2 = {105.0, 99.0}
    var (x, y) = Vector2.lineLineIntersect(a, b, c, d)
    Console.print(y)
}
