#global testerror "Err0569"

struct S
{
}

impl S
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(using self, stmt: s32)
    {
        for 10
        {
            #macro
            {
                var value = 1
                #mixin #up stmt
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