#global testerror "Err0022"

interface I
{
    mtd toto();
}

struct A {}
struct B {}
impl I for A
{
    mtd impl toto() {}
}

impl I for B
{
    mtd impl toto() {}
}

struct C
{
    using a: A
    using b: B
}

#test
{
    var c: C
    var i = cast(I) c
}