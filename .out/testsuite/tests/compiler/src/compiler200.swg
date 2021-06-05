;
;
using Swag;

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

#assert cast(f32) (main_u8()) == 45;
#assert cast(f32) (45'u8) == 45;
#assert cast(f32) (main_u16()) == 45;
#assert cast(f32) (45'u16) == 45;
#assert cast(f32) (main_u32()) == 45;
#assert cast(f32) (45'u32) == 45;
#assert cast(f32) (main_u64()) == 45;
#assert cast(f32) (45'u64) == 45;

#assert cast(f64) (main_u8()) == 45;
#assert cast(f64) (45'u8) == 45;
#assert cast(f64) (main_u16()) == 45;
#assert cast(f64) (45'u16) == 45;
#assert cast(f64) (main_u32()) == 45;
#assert cast(f64) (45'u32) == 45;
#assert cast(f64) (main_u64()) == 45;
#assert cast(f64) (45'u64) == 45;

#assert cast(f32) (main_s8()) == -45;
#assert cast(f32) (-45's8) == -45;
#assert cast(f32) (main_s16()) == -45;
#assert cast(f32) (-45's16) == -45;
#assert cast(f32) (main_s32()) == -45;
#assert cast(f32) (-45's32) == -45;
#assert cast(f32) (main_s64()) == -45;
#assert cast(f32) (-45's64) == -45;

#assert cast(f64) (main_s8()) == -45;
#assert cast(f64) (-45's8) == -45;
#assert cast(f64) (main_s16()) == -45;
#assert cast(f64) (-45's16) == -45;
#assert cast(f64) (main_s32()) == -45;
#assert cast(f64) (-45's32) == -45;
#assert cast(f64) (main_s64()) == -45;
#assert cast(f64) (-45's64) == -45;
