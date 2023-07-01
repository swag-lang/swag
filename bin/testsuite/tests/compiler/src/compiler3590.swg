interface I
{
    toto: func(self)
}

struct A
{
}

impl I for A
{
    mtd impl toto() {}
}

struct B
{
    using a: A
}

impl I for B
{
    mtd impl toto() {}
}

func(T) alloc()
{
    var x: *T
    itf := cast(I) x
}

#test
{
    alloc'B()
}