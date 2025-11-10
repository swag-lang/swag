//#global skip

struct A {}
interface I
{
    func fct(me);
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
    //#[Swag.PrintBc]
    mtd opAffect(v: const [..] T)
    {
        me.field = v[0]
        me.field.fct()
    }
}

//#[Swag.PrintBc]
#test
{
    var a: A
    var g: G'I = [a]
    @assert(GG == 1)
}
