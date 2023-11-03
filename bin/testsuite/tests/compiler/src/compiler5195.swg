struct A {x: s32}

func toto() throw
{
    throw A{42}
}

#test
{
    catch toto()
    let err = cast(A) @err()
    @assert(err.x == 42)
}