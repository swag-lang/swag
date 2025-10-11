#global testerror "Err0536"

struct S {}

impl S
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(me, stmt: s32)
    {
        for 10
        {
            #macro
            {
                var value = 1
                #inject(#up stmt)
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
