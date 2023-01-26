#global testerror

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
    countTot := 0
    loop parse()
    {
        countTot += 1
    }

    @assert(countTot == 5)
}

#test
{
    toto()
}