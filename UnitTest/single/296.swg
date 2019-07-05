#module module296;
#unittest pass semantic;

#run
{
	@assert(@prop(*bool, sizeof) == 8);
	@assert(@prop(1, sizeof) == 4);
	@assert(@prop(1'u64, sizeof) == 8);
	@assert(@prop(1.0, sizeof) == 4);
	@assert(@prop(1.0'f64, sizeof) == 8);
	@assert(@prop(5 * 4, sizeof) == 4);
}