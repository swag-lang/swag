

#global testerror;
#test
{
	var a: s8 = 127's8 + 1's8;
}

#global testerror;
#test
{
	var b: u8 = 255'u8 + 1'u8;
}

#assert 1.0 / 2.0 == 0.5;
