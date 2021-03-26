/*struct(T) PointBase
{
    x: T
    y: T
    const Zero: Self'T{1, 666}
}

#assert @typeof(Point.Zero.x) == s32
#assert Point.Zero.x == 1
#assert Point.Zero.y == 666

#assert @typeof(Pointf.Zero.x) == f32
#assert Pointf.Zero.x == 1.0
#assert Pointf.Zero.y == 666.0

alias Point = PointBase's32
alias Pointf = PointBase'f32*/
