#unittest module module130;
#unittest pass semantic;

attr toto: () -> var;

#unittest error;
#[toto]
{
	#[toto]
	{
		var b: bool;
	}
}
