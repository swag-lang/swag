var x: any = 666

//#[Swag.PrintBc]
#test
{
    let t = @kindof(x)
    @assert(t.fullname == "s32")
    let ptr = cast(*s32) @dataof(x)
    @assert(dref ptr == 666)
}
