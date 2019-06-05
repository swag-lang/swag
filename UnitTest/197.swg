#unittest module module197
#unittest pass semantic

#assert (45 as u8) as s8 == 45
#assert (45 as u16) as s8 == 45
#assert (0xFFFF as u16) as s8 == -1
#assert (45 as u32) as s16 == 45
#assert (0xFFFFFFFF as u32) as s8 == -1

#assert (45 as u8) as s16 == 45
#assert (45 as u16) as s16 == 45
#assert (0xFFFF as u16) as s16 == -1
#assert (45 as u32) as s16 == 45
#assert (0xFFFFFFFF as u32) as s16 == -1

#assert (45 as u8) as s32 == 45
#assert (45 as u16) as s32 == 45
#assert (0xFFFF as u16) as s32 == 0xFFFF as s32
#assert (45 as u32) as s32 == 45
#assert (0xFFFFFFFF as u32) as s32 == -1


#assert (45 as u8) as s64 == 45
#assert (45 as u16) as s64 == 45
#assert (0xFFFF as u16) as s64 == 0xFFFF as s64
#assert (45 as u32) as s64 == 45
#assert (0xFFFFFFFF as u32) as s64 == 0xFFFFFFFF as s64


