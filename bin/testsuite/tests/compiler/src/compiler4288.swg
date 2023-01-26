var G = 0

#[Swag.Macro]
func parse(stmt: code)->bool throw
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

#test
{
    cpt := 0
    countTot := 0
    while try parse()
    {
        countTot += 1
    }
}
