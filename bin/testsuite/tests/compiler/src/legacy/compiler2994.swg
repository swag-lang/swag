interface I
{
    func op(me);
}

//#[Swag.PrintBc]
#test
{
    var x: I
    if !x:
        @assert(true)
    else:
        @assert(false)
}