#global testerror "Err0471"

#[Swag.Macro]
func toto()->s32
{
}

#test
{
    let a = toto()
}