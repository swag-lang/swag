func toto() {}

//#[swag.printbc]
#test
{
    ptr := @typeof(toto)
    @assert(ptr.name == "__compiler2078.toto()")
}
