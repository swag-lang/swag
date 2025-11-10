struct Point { x, y: f32 }

func RR()->{ pt: Point, err: bool }
{
    return {{1, 2}, true}
}

#test
{
    let res = RR()
    @assert(res.item0.x == 1)
    @assert(res.item0.y == 2)
    @assert(res.item1 == true)
    @assert(res.err == true)
}
