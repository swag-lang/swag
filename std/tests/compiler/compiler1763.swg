/*//#[swag.printbc]
#test
{
    cpt := 0
    label outer
    {
        defer cpt += 1
        loop 10
        {
            @print("x ")
            break outer
        }
        //@assert(false)
    }

    @print(cast(s64) cpt)
    //@assert(cpt == 1)
}*/