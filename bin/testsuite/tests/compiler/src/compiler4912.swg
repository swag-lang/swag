#global testerror "Err0139"

#[Swag.Mixin]
func toto(stmt: code)
{
    for 5
    {
        #mixin stmt where { toto = titi; }
    }
}
