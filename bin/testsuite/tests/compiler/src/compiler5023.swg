#global testerror "Err0572"

#[Swag.Macro]
func toto()->s32
{
}

#test
{
    let a = toto()
}