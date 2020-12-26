namespace std.crypto
{
	struct Mwc
	{
		x: u32
	}

	impl Mwc
	{
		func seed(using self, _seed: u32)
		{
			x = _seed
		}
	}
}