alias T = func()->void

var GG: bool

func G()
{
    GG = true
}

//#[Swag.PrintBc]
#test
{
    GG = false

    var x: T
    if x do
        @assert(false)
    x = &G
    if !x do
        @assert(false)
    if x do
        x()

    @assert(GG)
}
