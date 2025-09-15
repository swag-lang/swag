using Core

struct A
{
    x:   s32
}

func toto(x: A)
{
    var buf: ConcatBuffer
    StrConv.convertAny(&buf, x, {}, "")
}

#test
{
    let a: A
    toto(a)
}
