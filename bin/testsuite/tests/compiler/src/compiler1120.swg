namespace A.B
{
	struct(T) Random
	{
		x: s32
	}
}

#test
{
	using A.B
	var rng: Random's32
}
