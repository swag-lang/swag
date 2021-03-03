#[swag.inline]
func res() => 666

#[swag.macro]
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