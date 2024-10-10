#global testerror "Err0051"

struct A
{
    drawingBegin: bool
}

#[Swag.Mixin, Swag.NoInline]
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