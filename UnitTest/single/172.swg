#module module172;
#unittest pass semantic;

#[swag.constexpr]
func main () -> s32
{
	return 666;
}

#assert main == 666;
#assert main() == 666;

