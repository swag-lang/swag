#test
{
    {
        let xx = 555
        let yy = 666
        let t  = {xx, yy}
    }

    {
        let x = 555
        let y = 666
        let t = {x, y}
        @assert(t.x == 555)
        @assert(t.item0 == t.x)
        @assert(t.y == 666)
        @assert(t.item1 == t.y)
    }
}
