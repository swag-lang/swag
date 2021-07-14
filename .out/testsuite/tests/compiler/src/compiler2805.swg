var G = 0

#[Swag.Macro]
func toto(stmt: code)
{
    #macro
    {
        x := 0's64
        defer G = 1
        #mixin `stmt
    }
}

#test
{
    toto()
    {
    }

    @assert(G == 1)
}