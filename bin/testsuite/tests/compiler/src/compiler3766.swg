#global testerror "Err0608"

struct A
{
    a: s32
}

func toto(p: *s32)
{
}

#test
{
    var x: A
    var ptr = cast(const *A) &x
    toto(&ptr.a)
}