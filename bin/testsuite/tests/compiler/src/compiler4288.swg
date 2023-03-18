var G = 0

#[Swag.Macro]
func parse(stmt: code) throw
{
    loop
    {
        G += 1
        if G == 5
            return
        #macro
        {
            #mixin #up stmt
        }
    }
}

#test
{
    cpt := 0
    countTot := 0
    try parse()
    {
        countTot += 1
    }

    @assert(countTot == 4)
}
