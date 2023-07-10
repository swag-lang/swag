struct S
{
    down:   s32 = 10
    up:     s32 = 20
}

impl S
{
    #[Swag.Mixin, Swag.CalleeReturn]
    func(ptr: bool) opVisit(using self, stmt: code)
    {
        for var v = down; v <= up; v += 1
        {
            let @alias0 = v
            let @alias1 = @index
            #mixin stmt

            /*#macro
            {
                let alias0 = x
                let alias1 = @index
                //#mixin #up stmt
            }*/
        }
    }
}

#test
{
    var x: S
    var cpt = 0
    var cpt1 = 0
    visit i: x
    {
        cpt +=  i
        cpt1 += cast(s32) @alias1
    }

    @assert(cpt == 165)
    @assert(cpt1 == 55)
}