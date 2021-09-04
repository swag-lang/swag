//#global testerror
/*struct AA
{
}

impl AA
{
    #[Swag.Macro]
    mtd(ptr: bool) opVisit(stmt: code)
    {
        var @alias0: s32
    }
}

func toto(x: AA)
{
    visit v: dref &x
    {
    }
}*/

#test
{
    var t = @[1, 2]
    var x: *s32 = &t
    y := dref &t[1]
    //@print(@typeof(y).name)
    @assert(y == 2)
}