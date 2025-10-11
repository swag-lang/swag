//#global skip
//#global testerror "Saf0016"

//#[Swag.PrintGenBc]
#[Swag.Sanity(true)]
func mkString(str: #null string)->#null string
{
    if str == null do
        return null

    let count = @countof(str)
    let ptr   = @getcontext().errorAllocator.alloc(count)
    @memcpy(ptr, @dataof(str), 5)
    return @mkstring(cast(const [*] u8) ptr, 5)
}
