struct T { a, b: s32; }

#[Swag.PrintBc]
func toto(t: T)->T
{
    return t
}

#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    x := toto({1, 2})
    @print(x.a, " ", x.b, "\n")
}