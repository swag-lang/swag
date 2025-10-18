#global testerror "Err0703"

#[Swag.Macro]
func toto()->s32 {}

#test
{
    let a = toto()
}
