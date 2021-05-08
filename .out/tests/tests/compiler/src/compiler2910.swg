func z_bit_reverse(n: u16, bits: u8) -> u16
{
	r := n
	r = ((r & 0xAAAA) >>  1) | ((r & 0x5555) << 1)
	r = ((r & 0xCCCC) >>  2) | ((r & 0x3333) << 2)
	r = ((r & 0xF0F0) >>  4) | ((r & 0x0F0F) << 4)
	r = ((r & 0xFF00) >>  8) | ((r & 0x00FF) << 8)

	r >>= (16 - bits)
    return r
}

func z_bit_reverse1(n: u16, bits: u8) -> u16
{
    r := 0'u16//@bitreverse(n)
    r >>= 16 - bits
    return r
}

#test
{

}