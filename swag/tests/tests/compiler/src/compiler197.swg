#[swag.safety("of", false)]
{
#assert cast(s8) (cast(u8) (45)) == 45;
#assert cast(s8) (cast(u8) (45)) == 45;
#assert cast(s8) (cast(u16) (0xFFFF)) == -1;
#assert cast(s8) (cast(u32) (45)) == 45;
#assert cast(s8) (cast(u32) (0xFFFFFFFF)) == -1;

#assert cast(s16) (cast(u8) (45)) == 45;
#assert cast(s16) (cast(u8) (45)) == 45;
#assert cast(s16) (cast(u16) (0xFFFF)) == -1;
#assert cast(s16) (cast(u32) (45)) == 45;
#assert cast(s16) (cast(u32) (0xFFFFFFFF)) == -1;

#assert cast(s32) (cast(u8) (45)) == 45;
#assert cast(s32) (cast(u8) (45)) == 45;
#assert cast(s32) (cast(u16) (0xFFFF)) == cast(s32) (0xFFFF);
#assert cast(s32) (cast(u32) (45)) == 45;
#assert cast(s32) (cast(u32) (0xFFFFFFFF)) == -1;

#assert cast(s64) (cast(u8) (45)) == 45;
#assert cast(s64) (cast(u8) (45)) == 45;
#assert cast(s64) (cast(u16) (0xFFFF)) == cast(s64) (0xFFFF);
#assert cast(s64) (cast(u32) (45)) == 45;
#assert cast(s64) (cast(u32) (0xFFFFFFFF)) == cast(s64) (0xFFFFFFFF);
}