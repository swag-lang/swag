#module module295;
using swag;
#unittest pass semantic;

#run
{
	@assert(@prop(bool, sizeof) == 1);
	@assert(@prop(s8, sizeof) == 1);
	@assert(@prop(s16, sizeof) == 2);
	@assert(@prop(s32, sizeof) == 4);
	@assert(@prop(s64, sizeof) == 8);
	@assert(@prop(u8, sizeof) == 1);
	@assert(@prop(u16, sizeof) == 2);
	@assert(@prop(u32, sizeof) == 4);
	@assert(@prop(u64, sizeof) == 8);	
	@assert(@prop(f32, sizeof) == 4);
	@assert(@prop(f64, sizeof) == 8);	
	@assert(@prop(char, sizeof) == 4);	
	@assert(@prop(string, sizeof) == 8 + 8);	
}