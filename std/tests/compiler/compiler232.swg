#module compiler232
#unittest pass semantic;
using swag;

#unittest error;
func main ()
{
	var a = true;
	a += true;
}

#unittest error;
func main1 ()
{
	var a = 'a';
	a += 'a';
}


