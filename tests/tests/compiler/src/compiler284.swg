;
;

//#[swag.printbc]
func main ()
{
	var v: bool = false;
	var p1: *s32 = cast(*s32)(&v);
}

#run 
{
	main()
}
