//#[swag.printbc]
#test
{
    {
        cpt := 0'u32
        loop 5
        {
            cpt += @index
        }

        @assert(cpt == 0+1+2+3+4)
    }

    {
        cpt := 0'u32
        loop 5
        {
            cpt += @index
        }

        @assert(cpt == 0+1+2+3+4)
    }
}
