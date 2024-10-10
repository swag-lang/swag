#global testerror "Err0049"

struct A
{
    drawingBegin: bool
}

#[Swag.Macro, Swag.NoInline]
func assert(expr: bool)
{
    if !expr:
        @print("toto")
}

impl A
{
    #[Swag.Inline]
    mtd newCommand()
    {
        assert(self.drawingBegin)
    }

    mtd toto()
    {
        newCommand()
    }
}

#test
{
    var a: A
    a.toto()
}