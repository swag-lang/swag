var G: s32

#[swag.compiler, swag.constexpr]
func toto()->string
{
    return @"G = 666"@
}

#test
{
    #ast toto()
    @assert(G == 666)
}