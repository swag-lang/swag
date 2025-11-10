struct Err { str: string }

func str(x: string)->string
{
    let cxt   = @getcontext()
    let count = cast(u64) @countof(x)
    let ptr   = cast([*] u8) cxt.errorAllocator.alloc(count)
    @memcpy(ptr, @dataof(x), count)
    return @mkstring(ptr, count)
}

func titi() throw
{
    throw Err{str("titi")}
}

//#[Swag.PrintBc]
func toto() throw
{
    defer catch titi()
    throw Err{str("toto")}
}

#test
{
    catch toto()
    @assert(@err == Err)
    let err = cast(Err) @err
    @assert(err.str == "toto")
}
