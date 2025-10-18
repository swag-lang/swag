#global testerror "Err0539"
struct A { x: s32 }

impl A
{
    mtd set(x: s32)
    {
        me.x = x
    }
}

struct B { a: A }

#test
{
    let b: B
    b.a.set(1)
}
