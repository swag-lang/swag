interface I
{
    func toto(self);
}

//#[Swag.PrintBc]
#test
{
    var a, b: I
    @assert(a == b)
}