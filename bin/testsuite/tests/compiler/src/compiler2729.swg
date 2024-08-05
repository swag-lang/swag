#global testerror "Err0538"

#[Swag.Inline]
func toto()->s32 throw
{
    return 0
}

#test
{
    var x = toto()
}