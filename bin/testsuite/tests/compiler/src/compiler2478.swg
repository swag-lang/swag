#global #[Swag.Safety("overflow", true)]
#global testerror "Saf0023"
#global testerror "Saf0024"

#assert cast(s8) 256 == 0
#assert cast(s8) 257 == 0
#assert cast(s64) Swag.U64.Max == 0
#assert cast(s16) Swag.U16.Max == 0
#assert cast(s32) Swag.U64.Max == 0
