struct AA
{
    x: f64 = 1
    y: f64 = 2
    z: f64 = 3
}

//#[swag.printbc]
func toto(x: AA)
{
    v := @{x}
    @assert(v.item0.x == 1)
    @assert(v.item0.y == 2)
    @assert(v.item0.z == 3)
}

#test
{
    var v: AA
    toto(v)
}