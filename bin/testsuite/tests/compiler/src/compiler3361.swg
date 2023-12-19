//#global skip

struct A
{
}

#[Swag.Macro]
func toto(x: s32, stmt: code)
{
    loop x
    {
        #macro
        {
            var #alias0 = #index
            #mixin #up stmt
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
    func(ptr: bool, back: bool) opVisit(using self, stmt: code)
    {
        toto(|#alias0| titi())
        {
            #mixin stmt
        }
    }
}

#test
{
    var v: A
    var xx = 0
    visit i: v
    {
        xx += i
    }

    @assert(xx == 45)
}