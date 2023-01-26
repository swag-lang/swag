var G = 0

#[Swag.Macro]
func parse(stmt: code)->bool
{
    G += 1
    if G == 5
        return false
    #macro
    {
        #mixin stmt
    }

    return true
}

func toto()
{
    countTot := 0
    while parse()
    {
        countTot += 1
    }

    @assert(countTot == 4)
}

#test
{
    toto()
}