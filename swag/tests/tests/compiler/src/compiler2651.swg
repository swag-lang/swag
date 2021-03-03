#[swag.inline]
func res() => 666

#[swag.macro]
func toto(stmt: code)
{
    #[swag.noreturn]
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