interface I
{
    op: func(self)
}

//#[Swag.PrintBc]
#test
{
    var x: I
    if x @assert(false)
}