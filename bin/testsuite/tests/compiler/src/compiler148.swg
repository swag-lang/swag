#global testerror "Err0515";

#[Swag.Overload]
func main (){}
#[Swag.Overload]
func main (b: bool){}

#run
{
	main(1, 2);
}

#[Swag.Overload]
func main1 (b: bool, b1: bool){}
#[Swag.Overload]
func main1 (b: string, b1: string){}
