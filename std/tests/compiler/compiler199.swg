;
;
using swag;

#[constexpr] 
{
	func main_u8  ()->u8  { return 45; }
	func main_u16 ()->u16 { return 45; }
	func main_u32 ()->u32 { return 45; }
	func main_u64 ()->u64 { return 45; }
	func main_s8  ()->s8  { return -45; }
	func main_s16 ()->s16 { return -45; }
	func main_s32 ()->s32 { return -45; }
	func main_s64 ()->s64 { return -45; }
	func main_f32 ()->f32 { return 1.5; }
	func main_f64 ()->f64 { return 1.5; }
}

#assert cast(s8) (main_f32()) == 1;
#assert cast(s8) (1.5'f32 ) == 1;
#assert cast(s8) (main_f64()) == 1;
#assert cast(s8) (1.5'f64 ) == 1;

#assert cast(s16) (main_f32()) == 1;
#assert cast(s16) (1.5'f32 ) == 1;
#assert cast(s16) (main_f64()) == 1;
#assert cast(s16) (1.5'f64 ) == 1;

#assert cast(s32) (main_f32()) == 1;
#assert cast(s32) (1.5'f32 ) == 1;
#assert cast(s32) (main_f64()) == 1;
#assert cast(s32) (1.5'f64 ) == 1;

#assert cast(s64) (main_f32()) == 1;
#assert cast(s64) (1.5'f32 ) == 1;
#assert cast(s64) (main_f64()) == 1;
#assert cast(s64) (1.5'f64 ) == 1;

