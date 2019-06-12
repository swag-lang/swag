#unittest module module172;
#unittest pass semantic;

#[swag.ConstExpr]
func main: () -> s32
{
	return 666;
}

#assert main == 666;
#assert main() == 666;

