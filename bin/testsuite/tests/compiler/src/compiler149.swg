#global testerror "Err0565";

#[Swag.Overload]
func main (){}
#[Swag.Overload]
func main (b: string){}

#run
{
	main(1);
}
