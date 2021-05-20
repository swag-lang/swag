;
;
using Swag;

#[constexpr]
func main_u8  ()->u8  { return 1; }

#assert main_u8() == main_u8();
