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
            var @alias0 = @index
            #mixin `stmt
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
    func(ptr: bool) opVisit(using self, stmt: code)
    {
        x := titi()
        toto(|@alias0| x)
        {
            #mixin stmt
        }
    }
}

#test
{
    var v: A
    visit v
    {
    }
}