#global testerror "Err0715"
struct A { z: s32; }

//#[Swag.PrintBc]
func toto(x: {a:s32})
{
    var v = x.x
}

//#[Swag.PrintBc]
#test
{
    //var y: A
    //toto(y)
    //toto({2'f32})
}