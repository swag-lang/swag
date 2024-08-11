#global testerror "Err0408"

#[Swag.Mixin]
func toto()
{
    var #mix: s32
    #mix += 1
}

#test
{
    toto()
}