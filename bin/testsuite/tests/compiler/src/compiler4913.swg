#global testerror "Err0061"

#[Swag.Mixin]
func toto(stmt: code)
{
    for 5
    {
        #mixin stmt where { }
    }
}