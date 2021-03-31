//#global testerror

//#[swag.printbc]
#test
{
    x := 0'u32
    x += 1
    x -= 1
    y := 31'u32
    y += 1
    x >>= y
    @print(cast(s64) x)
}