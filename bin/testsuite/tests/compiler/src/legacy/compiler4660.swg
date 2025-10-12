//#global testerror

func totoXXX(x: s32) {}

//#[Swag.Safety("", "", false)]
#test
{
    const ptr = #typeof(totoXXX)
    let ptr1  = &ptr.parameters[0]
    @assert(ptr1.name == "x")
}
