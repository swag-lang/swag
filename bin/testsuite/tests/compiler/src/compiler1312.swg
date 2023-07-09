//#[Swag.PrintBc]
#test
{
    var cpt = 0
    #scope outer
    {
        defer cpt = 1
        loop 10
            break outer
        @assert(false)
    }

    @assert(cpt == 1)
}