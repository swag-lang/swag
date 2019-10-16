;
;

enum A: string
{
	Red = "red";
	Green = "green";
	Blue = "blue";
}

enum B: f32
{
	Red = 1.0;
	Green = 2.0;
	Blue = 3.0;
}

enum C: u32
{
	Red = 0x00FF0000;
	Green = 0x0000FF00;
	Blue = 0x000000FF;
}

#assert C.Blue == 255;