struct S
{
    down:     s32 = 10
    up:       s32 = 20
}

impl S
{
    #[Swag.Macro, Swag.CalleeReturn]
    func(ptr: bool, back: bool) opVisit(me, stmt: #code void)
    {
        for var v = me.down; v <= me.up; v += 1
        {
            let #alias0 = v
            let #alias1 = @index
            #inject(stmt)

            /*#macro
            {
                let alias0 = x
                let alias1 = @index
                //#inject(#up stmt)
            }*/
        }
    }
}

#test
{
    var x:   S
    var cpt  = 0
    var cpt1 = 0
    foreach i in x
    {
        cpt += i
        cpt1 += cast(s32) #alias1
    }

    @assert(cpt == 165)
    @assert(cpt1 == 55)
}
