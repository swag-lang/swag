;
;

#[swag.constexpr]
func main () -> s32
{
	return 666;
}

#assert main() != 700;
#assert main() < 700;
#assert main() > 500;
#assert main() >= 666;
#assert main() <= 666;
#assert main() >= 665;
#assert main() <= 667;


