
;

func main (){}
func main (b: bool){}

#unittest error;
#run
{
	main(1, 2);
}

func main1 (b: bool, b1: bool){}
func main1 (b: string, b1: string){}

#unittest error;
#run
{
	main1(1);
}