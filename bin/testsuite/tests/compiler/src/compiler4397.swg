#global testerror "Err0366"

struct A { x, y: s32; }

#test
{
    var a: A
    var b = ref a
}