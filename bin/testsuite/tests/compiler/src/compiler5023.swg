#global testerror "Err0472"

#[Swag.Macro]
func toto()->s32 {}

#test
{
    let a = toto()
}
