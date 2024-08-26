#global testerror "Err0500"

#[Swag.Macro]
func toto()->s32
{
}

#test
{
    let a = toto()
}