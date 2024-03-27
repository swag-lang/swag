struct S
{
    down:   s32 = 10
    up:     s32 = 20
}

impl S
{
    #[Swag.Macro, Swag.CalleeReturn]
    func(ptr: bool, back: bool) opVisit(using self, stmt: code)
    {
        for var v = down; v <= up; v += 1
        {
            #macro
            {
                var #alias0 = #up v
                #mixin #up stmt
            }
        }
    }
}

#test
{
    var x: S
    var cpt = 0
    visit i: x
    {
        var xx = 666
        cpt += i
    }

    @assert(cpt == 165)
}