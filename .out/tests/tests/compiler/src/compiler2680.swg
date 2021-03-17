#test
{
    {
        xx := 555
        yy := 666
        t := @{xx, yy}
    }

    {
        x := 555
        y := 666
        t := @{x, y}
        @assert(t.x == 555)
        @assert(t.item0 == t.x)
        @assert(t.y == 666)
        @assert(t.item1 == t.y)
    }
}