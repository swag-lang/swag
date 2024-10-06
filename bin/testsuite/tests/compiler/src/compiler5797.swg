var G = 5

#[Swag.NoInline]
func toto()->nl *s32
{
    return &G
}

//#[Swag.PrintGenBc]
#test
{
    let a = assume toto()
    @assert(dref a == 5)
}