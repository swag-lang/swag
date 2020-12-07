using swag

#[attributeUsage(AttributeUsage.Function)]
attr toto(v: string);

#[toto("6")]
func main ()
{
}
