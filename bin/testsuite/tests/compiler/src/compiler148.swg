#global testerror;

func main (){}
func main (b: bool){}

#run
{
	main(1, 2);
}

func main1 (b: bool, b1: bool){}
func main1 (b: string, b1: string){}
