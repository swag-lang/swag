#[Swag.Mixin]
func toto()
{
    var #mixin0: s32
    #mixin0 += 1
    var #mixin1: s32
    #mixin1 += 1
}

#test
{
    toto()
    toto()
}
