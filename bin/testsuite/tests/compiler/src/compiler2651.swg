#[Swag.Inline]
func res() => 666

#[Swag.Macro]
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