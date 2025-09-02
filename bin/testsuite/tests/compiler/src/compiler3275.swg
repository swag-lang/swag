using Swag

#[Mixin]
func cpt()
{
    #ast
    {
        let itf = @compiler
        itf.compileString("var G: s32 = 555")
        return ";"
    }
}

func toto()
{
    cpt()
}

#test
{
    toto()
    @assert(G == 555)
}
