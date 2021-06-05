var G: s32

#[Swag.compiler, Swag.constexpr]
func toto()->string
{
    return @"G = 666"@
}

#test
{
    #ast toto()
    @assert(G == 666)
}