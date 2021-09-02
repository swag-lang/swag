struct S
{
    down:   s32 = 10
    up:     s32 = 20
}

impl S
{
    #[Swag.Macro, Swag.NoReturn]
    func(ptr: bool) opVisit(using self, stmt: code)
    {
        x := down
        @assert(x == 10)
    }
}

#test
{
    var x: S
    visit x
    {
        @assert(false)
    }
}