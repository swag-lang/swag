;
;

#test
{
	var a = 0;
	var b = 0;
	var pa1 = &a;
	var pa2 = &a;
	var pb = &b;
	@assert(pa1 == pa2);
	@assert(pb > pa1);
	@assert(pa1 < pb);
	@assert(pb >= pa1);
	@assert(pa1 <= pb);	
	@assert(pa1 != pb);	
}
