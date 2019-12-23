struct S
{
    down:   s32 = 10
    up:     s32 = 20
}

impl S
{
    #[swag.macro]
    func opVisit(using self, stmt: code)
    {
        for v := down; v <= up; v += 1
        {      
            #macro
            {
                let @alias0 = `v
                let @alias1 = @index
                #mixin `stmt
            }
        }
    }
}

#test
{
    var x: S
    cpt := 0
    cpt1 := 0
    visit i: x
    {
        //v := 0
        cpt +=  i
        cpt1 += cast(s32) @alias1
    }  

    @assert(cpt == 165)
    @assert(cpt1 == 55)
}