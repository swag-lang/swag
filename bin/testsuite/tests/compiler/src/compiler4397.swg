#global testerror "Err0213"

struct A { x, y: s32; }

#test
{
    var a: A
    var b =,ref a
}