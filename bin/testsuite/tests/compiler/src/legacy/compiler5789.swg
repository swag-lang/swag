#global testerror "Err0039"

struct A
{
    drawingBegin: bool
}

#[Swag.Mixin, Swag.NoInline]
func assert(expr: bool)
{
    if !expr do
        @print("toto")
}

impl A
{
    #[Swag.Inline]
    mtd newCommand()
    {
        assert(me.drawingBegin)
    }

    mtd toto()
    {
        me.newCommand()
    }
}

#test
{
    var a: A
    a.toto()
}
