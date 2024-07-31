#global testerror "Err0585"

#[Swag.Macro]
func toto()->s32
{
}

#test
{
    let a = toto()
}