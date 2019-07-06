#module module385;
//#unittest pass semantic;

//#[swag.printbytecode]
#test
{
	var cpt = 10'u32;
	var val = 0;
	loop(cpt)
	{
		// This shoudln't change the loop count, as 'loop' avaluates the number
		// only once
		cpt = 4;
		val += 1;
	}
	
	@assert(val == 10);
}
