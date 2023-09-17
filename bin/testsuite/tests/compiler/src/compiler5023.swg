#global testerror "Err0749"

#[Swag.Macro]
func toto()->s32
{
}

#test
{
    let a = toto()
}