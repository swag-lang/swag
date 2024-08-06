#global testerror "Err0528"

#[Swag.Macro]
func toto()->s32
{
}

#test
{
    let a = toto()
}