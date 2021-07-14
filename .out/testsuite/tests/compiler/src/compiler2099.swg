//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    c := 0'u8
    var d: u8 = (c | 1'u8)
    @assert(@typeof((c | 1'u8)) == u8)
}
