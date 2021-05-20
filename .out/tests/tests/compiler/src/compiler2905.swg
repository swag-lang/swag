func toto(x: s32)
{
}

//#[Swag.safety("", false)]
//#[Swag.printbc]
#test
{
    ptr := @typeof(toto)
    ptr1 := &ptr.parameters[0]
    @assert(ptr1.name == "x")
}