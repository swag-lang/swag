#global testerror "Err0566"

struct S
{
}

impl S
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(using self, stmt: s32)
    {
        loop 10
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