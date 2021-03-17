
#global testerror

struct S
{
    down:   s32 = 10
    up:     s32 = 20
}

impl S
{
    #[swag.macro]
    func(ptr: bool) opVisit(using self, stmt: code)
    {
        for v := down; v <= up; v += 1
        {
            #macro
            {
                let @alias0 = `v
                #mixin `stmt
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
        i = 0
        cpt += i
    }

    @assert(cpt == 165)
}