interface I
{
    func op(me);
}

//#[Swag.PrintBc]
#test
{
    var x: I
    if x do
        @assert(false)
}
