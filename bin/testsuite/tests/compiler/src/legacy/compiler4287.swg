var G = 0

#[Swag.Macro]
func parse(stmt: #code void)
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

func toto()
{
    var countTot = 0
    parse()
    {
        countTot += 1
    }

    @assert(countTot == 4)
}

#test
{
    toto()
}
