var G = 0

#[Swag.Macro]
func parse(stmt: code) throw
{
    loop
    {
        G += 1
        if G == 5 do
            return
        #macro
        {
            #mixin #up stmt
        }
    }
}

#test
{
    var cpt = 0
    var countTot = 0
    try parse()
    {
        countTot += 1
    }

    @assert(countTot == 4)
}
