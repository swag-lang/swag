//#[Swag.PrintBc]
#test
{
    var cpt = 0
    #scope(outer)
    {
        defer cpt = 1
        for 10 do
            break to outer
        @assert(false)
    }

    @assert(cpt == 1)
}
