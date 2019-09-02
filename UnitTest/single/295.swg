#module module295;
using swag;
#unittest pass semantic;

#run
{
	@assert(@op(bool, sizeof) == 1);
	@assert(@op(s8, sizeof) == 1);
	@assert(@op(s16, sizeof) == 2);
	@assert(@op(s32, sizeof) == 4);
	@assert(@op(s64, sizeof) == 8);
	@assert(@op(u8, sizeof) == 1);
	@assert(@op(u16, sizeof) == 2);
	@assert(@op(u32, sizeof) == 4);
	@assert(@op(u64, sizeof) == 8);	
	@assert(@op(f32, sizeof) == 4);
	@assert(@op(f64, sizeof) == 8);	
	@assert(@op(char, sizeof) == 4);	
	@assert(@op(string, sizeof) == 8 + 8);	
}