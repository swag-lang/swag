#global testerror "Err0028"

struct A
{
    count: s32
}

impl A
{
    #[Swag.Macro]
    mtd toto()
    {
        @print(count)
    }
}

private var count = 1

#test
{
    var x: A
    x.toto()
}