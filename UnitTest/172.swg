#unittest module module172;
#unittest pass semantic;

#[swag.constExpr]
func main: () -> s32
{
	return 666;
}

#assert main == 666;
#assert main() == 666;

