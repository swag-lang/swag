#module "compiler128"

#[swag.attributeUsage(swag.AttributeUsage.GlobalVariable)]
attr toto();
#[swag.attributeUsage(swag.AttributeUsage.Function)]
attr toto1();

#unittest error;
#[toto]
{
	#[toto1]
	func main () {}
}
