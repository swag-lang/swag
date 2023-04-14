struct A
{
    x, y: s32
}

const X4473 = 666

enum RGB
{
    A
    B
    C
}

func toto(a: s32)->A
{
    var v: RGB
    var result: retval
    result = {a, 2}
    return result
}

#test
{
    x := toto(1)
}