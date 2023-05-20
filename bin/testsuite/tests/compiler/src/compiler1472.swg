struct S
{
    down:   s32 = 10
    up:     s32 = 20
}

impl S
{
    #[Swag.Macro, Swag.CalleeReturn]
    func(ptr: bool) opVisit(using self, stmt: code)
    {
        for v := down; v <= up; v += 1
        {
            #macro
            {
                var @alias0 = #up v
                #mixin #up stmt
            }
        }
    }
}

#test
{
    var x: S
    cpt := 0
    visit i: x
    {
        xx := 666
        cpt += i
    }

    @assert(cpt == 165)
}