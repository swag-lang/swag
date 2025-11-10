//#[Swag.PrintBc]
func toto(x: string, y: ...)
{
    @assert(x == "1.0")

    let ptr = cast(*string) @dataof(y[0])
    @assert(dref ptr == "10")
    @assert(@kindof(y[0]) == #typeof(string))
    @assert(@kindof(y[1]) == #typeof(s32))
}

//#[Swag.PrintBc]
#test
{
    toto("1.0", "10", 20)
}
