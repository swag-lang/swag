#global testerror

struct A { x, y: s32; }

#test
{
    var a: A
    b := ref a
}