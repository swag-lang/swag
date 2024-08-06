#global testerror "Err0610"

struct A { x, y: s32; }

func toto(x: &A)
{
    x.x = 1
    x.y = 2
}

#test
{
    var a: A
    //var b =,ref a
    toto(a)
    @print(a.x, a.y)
}