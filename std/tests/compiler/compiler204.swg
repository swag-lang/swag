;
;
using swag;

#[constexpr] 
{
	func main_u8  ()->u8  { return 1; }
	func main_u16 ()->u16 { return 1; }
	func main_u32 ()->u32 { return 1; }
	func main_u64 ()->u64 { return 1; }
	func main_s8  ()->s8  { return -1; }
	func main_s16 ()->s16 { return -1; }
	func main_s32 ()->s32 { return -1; }
	func main_s64 ()->s64 { return -1; }
	func main_f32 ()->f32 { return 1.0; }
	func main_f64 ()->f64 { return 1.0; }
}

#assert -1.0 == -1's8;
#assert -1.0 == -1's16;
#assert -1.0 == -1's32;
#assert -1.0 == -1's64;
