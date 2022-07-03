func toto() {}

//#[Swag.PrintBc]
#test
{
    ptr := @typeof(toto)
    @assert(ptr.fullname == "__compiler2078.toto()")
}
