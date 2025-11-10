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
#test
{
    var a: A
    var b: B

    var i: I //= b

    //#[Swag.Complete]
    switch i
    {
    case A:
        @print("ptr.a")
    case B:
        @print("ptr.v")
    }
}
