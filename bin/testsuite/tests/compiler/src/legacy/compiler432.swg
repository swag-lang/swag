//#[Swag.PrintBc]
//#global testerror
#test
{
    let c: const [..] u8 = [0, 1]
    let d  = cast(const [..] u16) c
    @assert(@countof(d) == 1)
}
