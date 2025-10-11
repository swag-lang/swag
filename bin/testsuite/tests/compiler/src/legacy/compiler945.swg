//#[Swag.PrintBc]
func toto(x: f32, y: ...)
{
    //@assert(x == 1.0)

    let ptr = cast(*string) @dataof(y[0])
    @assert(dref ptr == "10")

    //var ptr1 = cast(*s32) @dataof(y[1])
    //@assert(ptr1[0] == 20)
}

//#[Swag.PrintBc]
#test
{
    toto(1.0, "10", 20)
}
