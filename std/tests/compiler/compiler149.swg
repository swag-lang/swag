#module compiler149
#unittest pass semantic;

#unittest error;
func main (){}
func main (b: string){}

#run 
{
	main(1);
}
