interface I
{
    op: func(self)
}

//#[Swag.printbc]
#test
{
    var x: I
    if x @assert(false)
}