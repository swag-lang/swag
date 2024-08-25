#global testerror "Err0125"

var G = 0

#[Swag.Macro]
func parse(stmt: code)->s32
{
    #macro
    {
        #mixin stmt
    }

    return 5
}

func toto()
{
    var countTot = 0
    for parse()
    {
        countTot += 1
    }

    @assert(countTot == 5)
}

#test
{
    toto()
}