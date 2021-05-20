#global testerror;

#[Swag.attributeUsage(Swag.AttributeUsage.GlobalVariable)]
attr toto();

#[toto]
func main ()
{
}

#[Swag.attributeUsage(Swag.AttributeUsage.Function)]
attr toto: ();

/*#global testerror;
#[toto1]
var a: bool;*/