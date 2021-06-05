#[Swag.inline]
func res() => 666

#[Swag.macro]
func toto(stmt: code)
{
    #mixin stmt
}

#test
{
    toto()
    {
        x := res()
        return
    }
}