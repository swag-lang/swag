struct A { x, y: s32 }

const X44734   = cast(s32) 0xFF
const X4473    = 665
const TOTO4473 = "toto"
var Y4473      = 666

enum RGB
{
    A
    B
    C
}

func toto(a: s32)->A
{
    var v:      RGB
    var result: retval
    result = {a, 2}
    return result
}

#test
{
    var x = toto(1)
}
