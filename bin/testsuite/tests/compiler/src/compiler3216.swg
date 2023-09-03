#global testerror "Err0113"
struct A {}
struct B {}

struct C {}
impl C
{
    mtd toto(x: A){}
    mtd toto(x: B){}
    mtd toto(x, y: s32){}
}

#test
{
    var c: C
    c.toto(1.0, 2.0)
}