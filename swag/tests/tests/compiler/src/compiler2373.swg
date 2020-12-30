#[swag.macro]
func toto(stmt: code)
{
    #macro
    {
        #[swag.noreturn]
        #mixin `stmt
    }
}

func titi()->s32
{
    toto()
    {
        return 666
    }

    return 0
}

#test
{
    x := titi()
    @assert(x == 666)
}