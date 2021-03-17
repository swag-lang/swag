;
;

#run
{
	@assert(@sizeof(*bool) == 8);
	@assert(@sizeof(1) == 4);
	@assert(@sizeof(1'u64) == 8);
	@assert(@sizeof(1.0) == 4);
	@assert(@sizeof(1.0'f64) == 8);
	@assert(@sizeof(5 * 4) == 4);
}