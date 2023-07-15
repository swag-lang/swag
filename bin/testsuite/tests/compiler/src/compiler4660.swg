#global skip

func toto(x: s32)
{
}

//#[Swag.Safety("", false)]
//#[Swag.PrintBc]
#test
{
    const ptr = @typeof(toto)
    const ptr1 = &ptr.parameters[0]
    @assert(ptr1.name == "x")
}