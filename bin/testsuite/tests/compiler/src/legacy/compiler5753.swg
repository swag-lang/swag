#global testerror "Err0222"

interface I {}
struct A { v = "a" }
struct B { v = "b" }
impl I for A {}
impl I for B {}

//#[Swag.PrintBc]
func toto(i: I)
{
    //#[Swag.Complete]
    switch i
    {
    case A: 
        @print("ptr.a")
    case B: 
        @print("ptr.v")
    case null + 1:
        @print("ptr.v")
    }
}

#test
{
    var i: I //= b
    toto(i)
}