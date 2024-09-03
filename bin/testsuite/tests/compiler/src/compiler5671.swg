#global skip

#[Swag.Mixin]
func toto(stmt: code)
{
    for 5
    {
        #inject stmt => { break = break; }
    }
}