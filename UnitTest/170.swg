#unittest module module170;
#unittest pass semantic;
using swag;

enum T : u16
{
	X = 1;
}

#assert T.X == 1;
#assert T.X != 2;