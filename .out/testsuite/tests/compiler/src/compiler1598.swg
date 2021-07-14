var G: s32

#[Swag.Compiler, Swag.ConstExpr]
func toto()->string
{
    return @"G = 666"@
}

#test
{
    #ast toto()
    @assert(G == 666)
}