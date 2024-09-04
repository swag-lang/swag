#global testerror "Err0263"

#[Swag.Mixin]
func toto()
{
    var #inject: s32
    #inject += 1
}

#test
{
    toto()
    toto()
}