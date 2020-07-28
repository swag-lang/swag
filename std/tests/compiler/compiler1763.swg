struct S
{
    x, y: f64
}

func toto(x: S)
{
    @print(x.x)
}

#[swag.printbc]
#test
{
     var pt: S
     pt.x, pt.y = 10
     @assert(pt.x == 10)
     @assert(pt.y == 10)
     toto(pt)
}