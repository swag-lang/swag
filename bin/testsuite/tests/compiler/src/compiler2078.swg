func toto() {}

//#[Swag.PrintBc]
#test
{
    ptr := @typeof(toto)
    @assert(ptr.name == "__compiler2078.toto()->void")
}
