//#global skip

interface I
{
}
struct A { v = "a" }
struct B { v = "b" }
impl I for A
{
}
impl I for B
{
}

//#[Swag.PrintBc]
func toto(i: #null I)
{
    //#[Swag.Complete]
    switch i
    {
    case A:
        @print("ptr.a")
    case B:
        @print("ptr.v")
    }
}

#test
{
    var i: I //= b
    toto(i)
}
