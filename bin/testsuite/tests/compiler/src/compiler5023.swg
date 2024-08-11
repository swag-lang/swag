#global testerror "Err0469"

#[Swag.Macro]
func toto()->s32
{
}

#test
{
    let a = toto()
}