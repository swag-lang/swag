#global skip

#[Swag.Mixin]
func toto(stmt: #block void)
{
    for 5
    {
        #inject stmt => { break = break; }
    }
}