#global testerror "Err0565"

#[Swag.Macro]
func toto()->s32
{
}

#test
{
    let a = toto()
}