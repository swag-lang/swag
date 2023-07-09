#test
{
    {
        var xx = 555
        var yy = 666
        var t = {xx, yy}
    }

    {
        var x = 555
        var y = 666
        var t = {x, y}
        @assert(t.x == 555)
        @assert(t.item0 == t.x)
        @assert(t.y == 666)
        @assert(t.item1 == t.y)
    }
}