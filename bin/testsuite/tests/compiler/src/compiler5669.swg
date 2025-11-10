struct A
{
    a:   f32 = 1.5
}

struct B
{
    b:   f32 = 3.5
}

//#[Swag.PrintBc]
func toto(x: any)
{
    switch x
    {
    case A:
        let ptr = cast(const &A) x
    //@print(ptr.a)
    case B:
        let ptr = cast(const &B) x
    }
}

#test
{
    var x: A
    toto(x)
    var y: B
    toto(y)
}
