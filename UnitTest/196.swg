#unittest module module196
#unittest pass semantic
using swag;

#[constexpr] 
{
	func main_u8: ()->u8 { return 45; }
	func main_u16: ()->u16 { return 45; }
	func main_u32: ()->u32 { return 45; }
	func main_u64: ()->u64 { return 45; }
	func main_s8: ()->s8 { return -45; }
	func main_s16: ()->s16 { return -45; }
	func main_s32: ()->s32 { return -45; }
	func main_s64: ()->s64 { return -45; }
	func main_f32: ()->f32 { return 1.5; }
	func main_f64: ()->f64 { return 1.5; }
}

#assert main_u8 as u8 == 45
#assert main_u8 as u16 == 45
#assert main_u8 as u32 == 45

#assert main_u8 as u64 == 45
#assert main_u16 as u64 == 45
#assert main_u32 as u64 == 45
#assert main_s8 as u64 == 0xFFFFFFFF_FFFFFFD3
#assert main_s16 as u64 == 0xFFFFFFFF_FFFFFFD3
#assert main_s32 as u64 == 0xFFFFFFFF_FFFFFFD3
