var x: [?] any = [6]

//#[Swag.PrintBc]
#test
{
    let t    = x[0]
    let type = @kindof(t)
    @assert(type.fullname == "s32")

    let ptr = cast(*s32) @dataof(x[0])
    @assert(dref ptr == 6)
}
