#global testerror "Err0548"

#[Swag.Inline]
func toto()->s32 throw
{
    return 0
}

#test
{
    var x = toto()
}