#unittest module module195;
#unittest pass semantic;
using swag;

#[ConstExpr]
func main: ()->u16
{
	return 45;
}

#assert cast(f32) main == 45;