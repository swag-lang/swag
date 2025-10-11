struct A
{
    b:   B
}

struct B {}

impl B
{
    mtd toto() {}
}

#[Swag.Warning("Wrn0003", .Enable)]
#test
{
    var a: A
    a.b.toto()
}
