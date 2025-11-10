#[Swag.NoInline]
func toto(x: s32) => x

#[Swag.NoInline]
#test
{
    @assert(toto(5) == 5)
}
