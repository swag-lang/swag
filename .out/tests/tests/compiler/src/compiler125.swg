#global testerror;

#[swag.attributeUsage(swag.AttributeUsage.GlobalVariable)]
attr toto();

#[toto]
func main ()
{
}

#[swag.attributeUsage(swag.AttributeUsage.Function)]
attr toto: ();

/*#global testerror;
#[toto1]
var a: bool;*/