//#[swag.printbc]
#test
{
    cpt := 0
    label outer
    {
        defer cpt = 1
        loop 10
            break outer
        @assert(false)
    }

    @assert(cpt == 1)
}