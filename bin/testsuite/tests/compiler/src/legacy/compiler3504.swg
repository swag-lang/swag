var G = 0

struct A
{
    n:   bool
}

struct B
{
    using a: A
}

struct C
{
    n:   bool
}

impl C
{
    mtd toto()
    {
        var b: B
        with b
        {
            me.n = true
        }
    }
}
