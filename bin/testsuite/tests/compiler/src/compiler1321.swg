#global testerror "Err0449"

#[Swag.Mixin]
func toto(x: code)
{
    #mixin x
    #mixin x
}

//#[Swag.PrintBc]
#test
{
    var cpt = 0
    toto(#code
    {
        #up cpt += 1
    }
)
    @assert(cpt == 2)
}
