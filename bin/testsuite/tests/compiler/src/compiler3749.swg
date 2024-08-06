#global testerror "Err0346"

#[Swag.Mixin]
func toto()
{
    var #mixin: s32
    #mixin += 1
}

#test
{
    toto()
    toto()
}