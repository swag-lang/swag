namespace random
{
	struct Mwc
	{
		x: 		u32 = 1234567
		crry: 	u32 = 1234567
	}
	
	impl Mwc
	{
		const mult: u32 = 2083801278
		const mhi = mult >> 16
		const mlo = mult & 0xFFFF
	
		func seed(using self, seed: u32)
		{
			x = seed
			crry = 1234567
		}
		
		func next(using self)->u32
		{
		    xhi := x >> 16
			xlo := x & 0xFFFF

			chi := crry >> 16
			clo := crry & 0xFFFF

			x    = (xlo * mlo) + clo
			axc  := (xlo * mhi) + (xhi * mlo) + chi + (x >> 16)
			crry = (xhi * mhi) + (axc >> 16)

			x = (x & 0xFFFF) + (axc << 16)
			return x
		}
	}
}