#global testerror "Err0299"

struct A { x, y: s32; }

#test
{
    var a: A
    var b =,ref a
}