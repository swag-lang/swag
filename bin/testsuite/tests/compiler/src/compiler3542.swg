//#global skip

struct(T) A
{
}
impl A
{
    mtd toto()
    {
    }
}

struct(T) B
{
    using a: A'*T
}

impl B
{
    mtd titi()
    {
        toto()
    }
}

#test
{
    var b: B's32
    b.titi()
}
