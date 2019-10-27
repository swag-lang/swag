namespace std
{
	#[swag.public]
	struct Mwc
	{
		x: 		u32 = 1234567
		carry: 	u32 = 1234567
	}
	
	impl Mwc
	{
		const mult: u32 = 2083801278
		const mhi = mult >> 16
		const mlo = mult & 0xFFFF
	
		#[swag.public]
		func seed(using self, seed: u32)
		{
			x = seed
			carry = 1234567
		}
		
		#[swag.public]
		func next(using self)->u32
		{
		    xhi := x >> 16
			xlo := x & 0xFFFF

			chi := carry >> 16
			clo := carry & 0xFFFF

			x    = (xlo * mlo) + clo
			axc  := (xlo * mhi) + (xhi * mlo) + chi + (x >> 16)
			carry = (xhi * mhi) + (axc >> 16)

			x = (x & 0xFFFF) + (axc << 16)
			return x
		}
	}
}