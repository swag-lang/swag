#global skip

#[Swag.Mixin]
func toto(stmt: code)
{
    for 5
    {
        #mixin stmt where { break = break; }
    }
}