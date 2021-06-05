using Swag
#[attributeUsage(AttributeUsage.Function)]
attr toto();

#[toto]
func main ()
{
}

#[attributeUsage(AttributeUsage.GlobalVariable)]
attr toto1();

#[toto1]
var a: bool;