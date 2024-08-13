#global testerror "Err0470"

#[Swag.Macro]
func toto()->s32
{
}

#test
{
    let a = toto()
}