struct A { x, y: s32 }
struct B { x, y: s32 }

struct(T) Array {}

impl Array
{
    mtd toto(x: T) {}
}

#test
{
    var a: Array'A
    var b: Array'B
    var c: Array'{ x, y: s32 }

    c.toto({0, 0})
}
