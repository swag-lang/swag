func toto() {}

//#[Swag.PrintBc]
#test
{
    let ptr = #typeof(toto)
    @assert(ptr.fullname == "__compiler2078.toto()")
}
