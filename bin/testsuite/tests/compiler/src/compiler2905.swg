func toto(x: s32)
{
}

//#[Swag.Safety("", false)]
//#[Swag.PrintBc]
#test
{
    ptr := @typeof(toto)
    ptr1 := &ptr.parameters[0]
    @assert(ptr1.name == "x")
}