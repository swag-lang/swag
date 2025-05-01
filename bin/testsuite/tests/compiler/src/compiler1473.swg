#global testerror "assertion failed"

struct S
{
    down:   s32 = 10
    up:     s32 = 20
}

impl S
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(using self, stmt: code void)
    {
        for var v = down; v <= up; v += 1
        {
            #macro
            {
                var #alias0 = #up v
                #inject #up stmt
            }
        }
    }
}

#test
{
    var x: S
    var cpt = 0
    foreach i in x
    {
        var xx = 666
        i = 0
        cpt += i
    }

    @assert(cpt == 165)
}