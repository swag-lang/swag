#global testerror "Err0420"

#[Swag.Mixin]
func toto()
{
    var #uniq: s32
    #uniq += 1
}

#test
{
    toto()
}