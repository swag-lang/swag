//#global testerror "Err0038"
//#global skip

var G = 0

struct A
{
    drawingBegin: bool
}

#[Swag.Macro]
func assert(expr: bool)
{
    if !expr do
        G = 2
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
