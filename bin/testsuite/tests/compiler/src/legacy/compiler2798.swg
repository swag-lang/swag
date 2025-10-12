//#global testerror

//#[Swag.PrintBc]
#[Swag.Safety("", "", true)]
#test
{
    var x = 0'u32
    x += 1
    x -= 1
    var y = 31'u32
    y += 1
    x >>= y
}
