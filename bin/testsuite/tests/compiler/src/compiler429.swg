


func T(color: COLOR)->s32
{
	res := 0;
	switch(color)
	{
	case COLOR.RED: 	res = 1;
	case COLOR.GREEN: 	res = 2;
	case COLOR.BLUE: 	res = 3;
	default: 			res = 4;
	}

	return res;
}

//#[Swag.PrintBc]
#test
{
	var res : s32 = T(COLOR.RED);
	@assert(res == 1);
	@assert(T(COLOR.GREEN) == 2);
	@assert(T(COLOR.BLUE) == 3);
	@assert(T(COLOR.WHITE) == 4);
	@assert(T(COLOR.BLACK) == 4);
}

enum COLOR
{
	RED;
	GREEN;
	BLUE;
	WHITE;
	BLACK;
}