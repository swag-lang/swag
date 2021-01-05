
#global testerror;

//#[swag.printbc]
#test
{
	var a: string = null;
    #[swag.safety(true)]
	if(a[0] == 1)
	{
	}
}