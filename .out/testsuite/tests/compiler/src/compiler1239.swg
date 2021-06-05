alias T = func()->void

var GG: bool

func G()
{
    GG = true
}

//#[Swag.printbc]
#test
{
    GG = false

    var x: T
    if(x)
        @assert(false)
    x = &G
    if(!x)
        @assert(false)
    if(x)
        x()

    @assert(GG)
}