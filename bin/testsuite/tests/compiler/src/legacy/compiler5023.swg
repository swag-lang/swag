#global testerror "Err0477"

#[Swag.Macro]
func toto()->s32 {}

#test
{
    let a = toto()
}
