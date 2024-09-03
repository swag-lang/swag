#global testerror "Err0139"

#[Swag.Mixin]
func toto(stmt: code)
{
    for 5
    {
        #inject stmt where { toto = titi; }
    }
}
