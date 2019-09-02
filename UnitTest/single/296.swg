#module module296;
#unittest pass semantic;

#run
{
	@assert(@op(*bool, sizeof) == 8);
	@assert(@op(1, sizeof) == 4);
	@assert(@op(1'u64, sizeof) == 8);
	@assert(@op(1.0, sizeof) == 4);
	@assert(@op(1.0'f64, sizeof) == 8);
	@assert(@op(5 * 4, sizeof) == 4);
}