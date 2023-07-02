interface I
{
    func op(self)
}

//#[Swag.PrintBc]
#test
{
    var x: I
    if !x
        @assert(true)
    else
        @assert(false)
}