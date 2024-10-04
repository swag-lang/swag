//#global testerror "Err0784"
#global skip

struct A
{
    drawingBegin: bool
}

#[Swag.Macro]
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