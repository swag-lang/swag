struct V { x, y: f32 }

func toto(x: &V)
{
    x.x = 1
    x.y = 10
}

//#[Swag.PrintBc]
#test
{
    var x: V
    toto(&x)
    @assert(x.x == 1)
    @assert(x.y == 10)
}
