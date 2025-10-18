//#global testerror "Err0576"
struct A { x: s32 }

func(T) toto(x: T)
{
    var a: A = x
}

//#[Swag.PrintBc]
#test
{
    toto(A{1's32})
    //toto({2'f32})
}
