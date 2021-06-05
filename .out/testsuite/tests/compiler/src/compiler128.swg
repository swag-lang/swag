#global testerror;

#[Swag.attributeUsage(Swag.AttributeUsage.GlobalVariable)]
attr toto();
#[Swag.attributeUsage(Swag.AttributeUsage.Function)]
attr toto1();

#[toto]
{
	#[toto1]
	func main () {}
}
