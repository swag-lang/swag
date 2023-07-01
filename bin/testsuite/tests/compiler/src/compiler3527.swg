struct A {}
interface I
{
    fct: func(self)
}

var GG = 0

impl I for A
{
    mtd impl fct()
    {
        GG = 1
    }
}

struct(T) G
{
    field: T
}

impl G
{
    mtd opAffect(v: const [..] T)
    {
        field = v[0]
        field.fct()
    }
}

//#[Swag.PrintBc]
#test
{
    var a: A
    var g: G'I = [cast(I) a]
    @assert(GG == 1)
}