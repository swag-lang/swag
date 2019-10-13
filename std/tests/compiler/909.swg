

func sum(x, y: f32 = 10) => x + y

#test
{
	@assert(sum() == 20)
	@assert(sum(1) == 11)
	@assert(sum(1, 3) == 4)
}