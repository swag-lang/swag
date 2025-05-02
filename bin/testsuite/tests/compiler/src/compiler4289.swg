#global testerror "Err0127"

var G = 0

#[Swag.Macro]
func parse(stmt: #code void)->s32
{
    #macro
    {
        #inject stmt
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