;
;

enum A
{
	Red = B.Red;
	Green;
	Blue;
}

enum B
{
	Red = 100;
	Green;
	Blue;
}

#assert A.Blue == 102;