#module module429
//#unittest pass semantic

enum COLOR
{
	RED;
	GREEN;
	BLUE;
}

func T(color: COLOR)->s32
{
	res := 0;
	switch(color)
	{
	case COLOR.RED: 	res = 1;
	case COLOR.GREEN: 	res = 2;
	case COLOR.BLUE: 	res = 3;
	default: 			res = 0;
	}
	
	return res;
}

//#[swag.printbytecode]
#test
{
}