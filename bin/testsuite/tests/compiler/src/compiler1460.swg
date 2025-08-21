struct S
{
    down:   s32 = 10
    up:     s32 = 20
}

impl S
{
    #[Swag.Macro, Swag.CalleeReturn]
    func(ptr: bool, back: bool) opVisit(using me, stmt: #code void)
    {
        let x = down
        @assert(x == 10)
    }
}

#test
{
    var x: S
    foreach x
    {
        @assert(false)
    }
}