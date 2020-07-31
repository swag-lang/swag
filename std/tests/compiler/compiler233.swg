using swag

var ga: u32 = 0

func main (off: u32)
{
	ga += off
}

#test
{
	main(1'u32)
	main(1'u32)
	main(4'u32)
	@assert(ga == 6)
}