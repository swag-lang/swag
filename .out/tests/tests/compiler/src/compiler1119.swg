struct(T /*= bool*/) Random
{
}

impl Random
{
	func nextS64(using self)->u64 => nextU64(self)
	func nextU64(using self)->u64 => 1
}


#test
{
	var rng: Random'bool
	r := rng.nextS64()
}
