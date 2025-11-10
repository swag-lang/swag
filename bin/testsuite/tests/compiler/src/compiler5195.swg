struct A { x: s32 }

func toto() throw
{
    throw A{42}
}

#test
{
    catch toto()
    @assert(@err == A)
    let err = cast(A) @err
    @assert(err.x == 42)
}
