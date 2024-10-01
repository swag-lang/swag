//#global skip
//#global testerror "San0002"

//#[Swag.PrintGenBc]
#[Swag.Sanity(true)]
func mkString(str: nl string)->string
{
    if str == null:
        return null

    let count = @countof(str)
    let ptr   = @getcontext().errorAllocator.alloc(count)
    @memcpy(ptr, @dataof(str), 5)
    return @mkstring(cast(const ^u8) ptr, 5)
}
