#global testerror "Err0576"

#[Swag.Macro]
func toto()->s32
{
}

#test
{
    let a = toto()
}