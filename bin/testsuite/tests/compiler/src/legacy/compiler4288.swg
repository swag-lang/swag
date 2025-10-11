var G = 0

#[Swag.Macro]
func parse(stmt: #code void) throw
{
    for
    {
        G += 1
        if G == 5 do
            return
        #macro
        {
            #inject(#up stmt)
        }
    }
}

#test
{
    var cpt      = 0
    var countTot = 0
    try parse()
    {
        countTot += 1
    }

    @assert(countTot == 4)
}
