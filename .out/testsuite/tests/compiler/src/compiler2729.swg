#global testerror

#[Swag.Inline]
func toto()->s32 throw
{
    return 0
}

#test
{
    x := toto()
}