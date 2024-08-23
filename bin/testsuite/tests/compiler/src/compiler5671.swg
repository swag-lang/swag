#global skip

#[Swag.Mixin]
func toto(stmt: code)
{
    loop 5
    {
        #mixin stmt where { break = break; }
    }
}