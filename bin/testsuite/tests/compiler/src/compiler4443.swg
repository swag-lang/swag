#global testerror

struct Pt { x, y: f32; }

interface I
{
    mtd toto(y: Pt)
}

struct A
{
}

impl I for A
{
    mtd toto(y: Pt)
    {
        @print(y.x)
    }
}

struct B
{
    using a: A
}

impl I for B
{
    mtd toto(y: const ref Pt)
    {
        a.I.toto(y)
    }
}

#test
{
    var b: B
    var i = cast(I) b

    var pt: Pt
    i.toto(pt)
}