#[swag.mixin, swag.noreturn]
func mac(stmt: code)
{
    #mixin stmt
}

func toto()->s32
{
    mac()
    {
        return 1
    }

    @assert(false)
}

#test
{
    x := toto()
    @assert(x == 1)
}