#global testerror "Err0544"

#[Swag.Inline]
func toto()->s32 throw
{
    return 0
}

#test
{
    var x = toto()
}