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
            let #alias0 = v
            let #alias1 = #index
            #mixin stmt

            /*#macro
            {
                let alias0 = x
                let alias1 = #index
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
    visit i, j in x
    {
        cpt +=  i
        cpt1 += cast(s32) j
    }

    @assert(cpt == 165)
    @assert(cpt1 == 55)
}