//#global testerror
var G = 0

struct A
{
    n: bool
}

struct B
{
    using b: A
}

struct C
{
    n: bool
}

impl C
{
    mtd toto()
    {
        var b: B
        with b
        {
            b.n = true
        }
    }
}
