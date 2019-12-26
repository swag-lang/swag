var G = 0

#[swag.macro]
func(op: bool) toto()
{
    #if op
        G += 1
    #else
        G += 2
}

#test
{
    G = 0
    toto!true()
    toto!false()
    @assert(G == 3)
}