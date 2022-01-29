//#[Swag.PrintBc]
func toto(v: closure(s32)->s32)->s32
{
    return v(5)
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    v := toto(@(x: s32)->s32 {
        return x
    })
    @print(v)
    //@assert(v == 5)
}