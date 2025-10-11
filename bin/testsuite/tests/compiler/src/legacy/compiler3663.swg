interface I
{
    func toto(me);
}

//#[Swag.PrintBc]
#test
{
    var a, b: I
    @assert(a == b)
}
