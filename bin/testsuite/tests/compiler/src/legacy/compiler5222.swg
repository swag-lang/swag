struct Err
{
    str: string
}

func toto() throw
{
    var x:  Err
    let ptr = @getcontext().errorAllocator.alloc(10)
    @memcpy(ptr, @dataof("coucou"), 6)
    x.str = @mkstring(cast(const [*] u8) ptr, 6)
    throw x
}

func toto1() throw
{
    catch toto()

    let err = cast(Err) @err
    var x:  Err
    x.str = err.str
    throw x
}

#test
{
    catch toto1()
    let err = cast(Err) @err
    @assert(err.str == "coucou")
}
