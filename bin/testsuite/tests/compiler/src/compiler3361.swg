//#global skip

struct A
{
}

#[Swag.Macro]
func toto(x: s32, stmt: #block void)
{
    for x
    {
        #macro
        {
            var #alias0 = #index
            #inject #up stmt
        }
    }
}

impl A
{
    #[Swag.Inline]
    mtd titi()->s32
    {
        return 10
    }

    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(using self, stmt: #block void)
    {
        toto(|#alias0| titi())
        {
            #inject stmt
        }
    }
}

#test
{
    var v: A
    var xx = 0
    foreach i in v
    {
        xx += i
    }

    @assert(xx == 45)
}