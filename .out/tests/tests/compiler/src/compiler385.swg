;
;

//#[Swag.printbc]
#test
{
	var cpt = 10'u32;
	var val = 0;
	loop(cpt)
	{
		// This shouldn't change the loop count, as 'loop' avaluates its expression
		// only once
		cpt = 4;
		val += 1;
	}

	@assert(val == 10);
}
