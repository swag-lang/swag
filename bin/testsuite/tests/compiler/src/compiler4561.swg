#global testerror

#[Swag.Safety("", true)]
#test
{
    var x: const [..] u8 = [0'u8, 1, 2]
    y := cast(const [..] u32) x
    @print(y[0])
}