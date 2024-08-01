#[Swag.Mixin]
func toto()
{
    var #mix0: s32
    #mix0 += 1
}

#test
{
    toto()
    toto()
}