#[Swag.Mixin]
func toto()
{
    var #uniq0: s32
    #uniq0 += 1
    var #uniq1: s32
    #uniq1 += 1
}

#test
{
    toto()
    toto()
}
