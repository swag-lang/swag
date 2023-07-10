#[Swag.Mixin, Swag.CalleeReturn]
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
    let x = toto()
    @assert(x == 1)
}