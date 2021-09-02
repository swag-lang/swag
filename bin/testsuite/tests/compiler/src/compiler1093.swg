#[Swag.EnumFlags]
enum DrawFilters
{
	None
	F2
	F3
}

func toto(drawFilters: DrawFilters = DrawFilters.None)
{
	using DrawFilters
	@assert(drawFilters & F2 != 0)
	@assert(drawFilters & F3 != 0)
	if(drawFilters & F2)
	{
	}
}

#test
{
	toto(DrawFilters.F2 | DrawFilters.F3)
}
