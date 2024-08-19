#global testerror "Err0479"

#[Swag.Macro]
func toto()->s32
{
}

#test
{
    let a = toto()
}