#unittest module module171
#unittest pass semantic
using swag;

enum T : s16
{
	X = -2;
	Y;
}

#assert T.X == -30
#assert T.Y == -1
