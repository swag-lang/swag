#global testerror "Saf0009";

#[Swag.Safety("", true)]
#test
{
	var a: s8 = 127's8 + 1's8;
}
