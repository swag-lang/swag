#module "compiler125"

#[swag.attributeUsage(swag.AttributeUsage.GlobalVariable)]
attr toto();

#unittest error;
#[toto]
func main ()
{
}

#[swag.attributeUsage(swag.AttributeUsage.Function)]
attr toto: ();

/*#unittest error;
#[toto1]
var a: bool;*/