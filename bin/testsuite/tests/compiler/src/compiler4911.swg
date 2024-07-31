#global testerror "Err0519"

#[Swag.Mixin]
func toto()
{
    var #alias: s32
    #alias += 1
}

#test
{
    toto()
}