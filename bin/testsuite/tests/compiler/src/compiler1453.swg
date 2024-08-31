#global testerror "Err0580"

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

    foreach x
    {
        @print("toto\n")
    }
}