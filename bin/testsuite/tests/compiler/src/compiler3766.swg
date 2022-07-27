#global testerror

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
    ptr := cast(const *A) &x
    toto(&ptr.a)
}