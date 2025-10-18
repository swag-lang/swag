//#global testerror "Err0017"
struct A { x: s32 }

var g: A
impl A
{
    mtd const toto()->*A
    {
        return &g
    }
}

#test
{
    var a: A
    a.toto().x = 0
    //@assert(b == 0)
}
