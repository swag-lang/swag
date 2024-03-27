;
;
using Swag;

#[ConstExpr]
func main_u8()->u8  { return 1; }
#[ConstExpr]
func main_f32()->f32  { return 1.0; }

#assert main_u8() == main_f32();
