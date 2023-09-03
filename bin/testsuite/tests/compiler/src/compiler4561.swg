#global testerror "Saf0008"

#[Swag.Safety("", true)]
#test
{
    let x: const [..] u8 = [0'u8, 1, 2]
    let y = cast(const [..] u32) x
    @print(y[0])
}