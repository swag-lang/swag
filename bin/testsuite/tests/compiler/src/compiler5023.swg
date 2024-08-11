#global testerror "Err0464"

#[Swag.Macro]
func toto()->s32
{
}

#test
{
    let a = toto()
}