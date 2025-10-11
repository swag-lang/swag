using Swag

#[Swag.Macro, Swag.ConstExpr]
func titi()->s32
{
    return 55
}

#[Swag.Macro, Swag.ConstExpr]
func toto()->s32
{
    return titi()
}

#[Swag.NoInline]
func tata()
{
    @assert(toto() == 55)
}

struct A
{
    a    = toto()
}

#test
{
    var a: A
    @assert(a.a == 55)
}
