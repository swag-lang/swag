;
;
using Swag;

#[ConstExpr]
func main ()->u16
{
	return 45;
}

#assert cast(f32) (main()) == 45;
