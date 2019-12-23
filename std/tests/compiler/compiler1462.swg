struct S
{
    down:   s32 = 10
    up:     s32 = 20
}

impl S
{
    #[swag.mixin]
    func opVisit(using self, stmt: code)
    {
        for v := down; v <= up; v += 1
        {
            let @alias0 = v
            let @alias1 = index
            #mixin stmt
        
            /*#macro
            {
                let alias0 = x
                let alias1 = index
                //#mixin `stmt
            }*/
        }
    }
}

#test
{
    var x: S    
    cpt := 0
    cpt1 := 0
    visit i, j: x
    {
        cpt +=  i
        cpt1 += cast(s32) j
    }  

    @assert(cpt == 165)
    @assert(cpt1 == 55)
}