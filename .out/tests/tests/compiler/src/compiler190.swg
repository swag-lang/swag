;
;

#[Swag.constexpr]
func main ()-> u32
{
	return 0b1111;
}

#assert main() ^ 0b1001 == 0b0110;
#assert main() ^ main() == 0;
