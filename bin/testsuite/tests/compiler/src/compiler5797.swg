var G = 5

#[Swag.NoInline]
func toto()->#null *s32
{
    return &G
}

//#[Swag.PrintGenBc]
#test
{
    let a = assume toto()
    @assert(dref a == 5)
}
