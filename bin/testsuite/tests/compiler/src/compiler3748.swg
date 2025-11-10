#[Swag.Mixin]
func toto()
{
    var #uniq0: s32
    #uniq0 += 1
}

#test
{
    toto()
    toto()
}
