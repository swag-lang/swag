func totoXXX(x: s32) {}

//#[Swag.Safety(.All, .All, false)]
//#[Swag.PrintBc]
#test
{
    let ptr  = #typeof(totoXXX)
    let ptr1 = &ptr.parameters[0]
    @assert(ptr1.name == "x")
}
