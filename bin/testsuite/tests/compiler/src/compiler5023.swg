#global testerror "Err0580"

#[Swag.Macro]
func toto()->s32
{
}

#test
{
    let a = toto()
}