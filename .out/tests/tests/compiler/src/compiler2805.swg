var G = 0

#[swag.macro]
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