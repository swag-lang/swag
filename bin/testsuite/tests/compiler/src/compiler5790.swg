//#global testerror "Err0051"
//#global skip

var G = 0

struct A
{
    drawingBegin: bool
}

#[Swag.Macro]
func assert(expr: bool)
{
    if !expr:
        G = 2
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