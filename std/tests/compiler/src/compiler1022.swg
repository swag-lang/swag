#module "compiler1022"
#unittest pass syntax

#test
{
	@init(ptr)
	@init(ptr + 10)
	@init(ptr, 10)
	@init(ptr, 10)(1, 2)

	@drop(ptr)
	@drop(ptr, a)
}