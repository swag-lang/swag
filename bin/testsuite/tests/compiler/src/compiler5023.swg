#global testerror "Err0530"

#[Swag.Macro]
func toto()->s32
{
}

#test
{
    let a = toto()
}