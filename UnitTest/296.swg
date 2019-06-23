#module module296;
using swag;
#unittest pass semantic;

#run
{
	@assert(@sizeof(*bool) == 8);
}