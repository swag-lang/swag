;
;
using Swag;

enum T : u16
{
	X = 1;
}

#assert T.X == 1;
#assert T.X != 2;