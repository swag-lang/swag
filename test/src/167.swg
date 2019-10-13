#module module167;
#unittest pass semantic;
using swag;

#run 
{
	@assert(1 == 1);
}

#run 
{
	@assert(2 > 1);
}

#run 
{
	@assert(!false);
}
