#test
{
    cpt1 := 0'uint
    cpt2 := 0'uint
    visit i: "toto"
    {
        cpt1 += @alias1
        visit j: "titi"
        {
            cpt2 += @alias1
        }
    }

    @assert(cpt1 == 6)
    @assert(cpt2 == 24)
}