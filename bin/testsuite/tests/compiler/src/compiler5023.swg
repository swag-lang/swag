#global testerror "Err0531"

#[Swag.Macro]
func toto()->s32
{
}

#test
{
    let a = toto()
}