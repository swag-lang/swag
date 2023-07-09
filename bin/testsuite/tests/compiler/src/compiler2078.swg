func toto() {}

//#[Swag.PrintBc]
#test
{
    var ptr = @typeof(toto)
    @assert(ptr.fullname == "__compiler2078.toto()")
}
