#global testerror "Err0376";

#[Swag.AttrUsage(Swag.AttributeUsage.GlobalVariable)]
attr toto();
#[Swag.AttrUsage(Swag.AttributeUsage.Function)]
attr toto1();

#[toto]
{
	#[toto1]
	func main () {}
}
