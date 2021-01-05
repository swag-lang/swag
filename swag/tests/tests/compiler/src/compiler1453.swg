
#global testerror

struct S
{
}

impl S
{
    #[swag.macro]
    func(ptr: bool) opVisit(using self, stmt: s32)
    {
        loop 10
        {
            #macro
            {
                value := 1
                #mixin `stmt
            }
        }
    }
}

#test
{
    var x: S

    visit x
    {
        @print("toto\n")
    }
}