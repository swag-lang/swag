var x: any = "666"

//#[Swag.PrintBc]
#test
{
    let t = @kindof(x)
    @assert(t.fullname == "string")
    let ptr = cast(*string) @dataof(x)
    @assert(dref ptr == "666")
}
