#global testerror "Err0507"

#[Swag.Macro]
func toto()->s32
{
}

#test
{
    let a = toto()
}