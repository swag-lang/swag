#global testerror "Err0579"

#[Swag.Macro]
func toto()->s32
{
}

#test
{
    let a = toto()
}
