#global testerror

#[swag.inline]
func toto()->s32 throw
{
    return 0
}

#test
{
    x := toto()
}