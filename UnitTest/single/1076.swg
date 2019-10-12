#module module1076

#test
{
	@assert(@countof(@arguments()) == 2)
	args := @arguments()
	@print(args[0])
	@print(args[1])
}