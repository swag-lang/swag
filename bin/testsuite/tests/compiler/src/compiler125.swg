#global testerror "Err0073";

#[Swag.AttrUsage(Swag.AttributeUsage.GlobalVariable)]
attr toto();

#[toto]
func main ()
{
}

#[Swag.AttrUsage(Swag.AttributeUsage.Function)]
attr toto: ();

/*#global gfrrfr;
#[toto1]
var a: bool;*/