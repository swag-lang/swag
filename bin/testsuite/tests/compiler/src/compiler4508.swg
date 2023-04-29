struct T { a, b: s32; }

//#[Swag.PrintBc]
#[Swag.NoInline]
func toto(t: T)->T
{
    //@print(t.a, " ", t.b, "\n")
    return t
}

//#[Swag.PrintBc]
//#[Swag.Optim("", false)]
#test
{
    x := toto({1, 2})
    //@print(x.a, x.b)
}