#global testerror

#[Swag.inline]
func toto()->s32 throw
{
    return 0
}

#test
{
    x := toto()
}