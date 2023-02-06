using Core

struct A { x: s32; }

func toto(x: A)
{
    var buf: ConcatBuffer
    StrConv.convertAny(&buf, x, "")
}

#test
{
    var a: A
    toto(a)
}