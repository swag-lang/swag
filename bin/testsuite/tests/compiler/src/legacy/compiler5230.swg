struct A { x, y: s32 }
struct B { x, y: s32 }

struct(T) Array {}

#test
{
    var a: Array'A
    var b: Array'B
    var c: Array'{ x, y: s32 }
}
