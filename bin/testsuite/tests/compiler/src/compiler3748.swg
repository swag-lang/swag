#[Swag.Mixin]
func toto()
{
    var #mixin0: s32
    #mixin0 += 1
}

#test
{
    toto()
    toto()
}