func toto()-> s32 { return -1; }
func titi()-> s32 { return 1; }

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    x := {toto(), 10}
    @assert(x.item0 == -1)
    @assert(x.item1 == 10)

    y := {toto(), titi()}
    @assert(y.item0 == -1)
    @assert(y.item1 == 1)

    z := {a = toto(), b = titi()}
    @assert(z.a == -1)
    @assert(z.b == 1)
}