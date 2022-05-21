interface I
{
    toto: func(self)
}

//#[Swag.PrintBc]
#test
{
    var a, b: I
    @assert(a == b)
}