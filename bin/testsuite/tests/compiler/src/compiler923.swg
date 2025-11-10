//#[Swag.PrintBc]
func average(x, y: any)
{
    let tx = cast(*s32) @dataof(x)
    @assert(dref tx == 5)
    let ty = cast(*s32) @dataof(y)
    @assert(dref ty == 10)
}

//#[Swag.PrintBc]
#test
{
    average(5, 10)
}
