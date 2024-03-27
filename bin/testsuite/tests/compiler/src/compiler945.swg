
//#[Swag.PrintBc]
func toto(x: f32, y: ...)
{
    //@assert(x == 1.0)

    let ptr = cast(*string) @dataof(y[0])
    @assert(dref ptr == "10")
}

//#[Swag.PrintBc]
#test
{
    toto(1.0, "10", 20)
}
