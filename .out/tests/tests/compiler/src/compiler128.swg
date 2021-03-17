#global testerror;

#[swag.attributeUsage(swag.AttributeUsage.GlobalVariable)]
attr toto();
#[swag.attributeUsage(swag.AttributeUsage.Function)]
attr toto1();

#[toto]
{
	#[toto1]
	func main () {}
}
