#global testerror "Saf0029"

interface I {}
struct A { v = "a" }
struct B { v = "b" }
impl I for A {}
impl I for B {}

//#[Swag.PrintBc]
#[Swag.Safety("", true)]
func toto(i: #null I)
{
    #[Swag.Complete]
    switch i
    {
    case A:
        @print("ptr.a")
    case B:
        @print("ptr.v")
    //case null:
      //  @print("ptr.v")
    }
}

#test
{
    var i: I //= b
    toto(i)
}