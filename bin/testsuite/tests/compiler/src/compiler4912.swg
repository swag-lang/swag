#global testerror "Err0118"

#[Swag.Mixin]
func toto(stmt: code)
{
    loop 5
    {
        #mixin stmt where { toto = titi; }
    }
}
