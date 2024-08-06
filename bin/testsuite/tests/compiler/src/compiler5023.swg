#global testerror "Err0524"

#[Swag.Macro]
func toto()->s32
{
}

#test
{
    let a = toto()
}