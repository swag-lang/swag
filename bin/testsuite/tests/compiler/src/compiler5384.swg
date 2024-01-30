#global skip
var G: rune = `A`
func toto()->const &rune
{
    return &G
}

#test
{
    let a = cast(rune) toto()
/*
    switch toto()
    {
    case `A`: break
    }*/
}