;
;
using swag;

enum T : s16
{
	X = -2;
	Y;
}

#assert T.X == -2;
#assert T.Y == -1;
