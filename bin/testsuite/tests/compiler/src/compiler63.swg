;
;

#assert 1'u8 + 255'u32 == 256;
#assert 1'u8 + 255'u64 == 256;

#assert 1 + 2.0'f32 == 3.0;
#assert 1 + 2.0'f32 == 3.0'f64;

#assert 10's8 + 2.0'f32 == 12;
#assert 2.0'f32 + 10's8 == 12;
#assert 10's8 + 2.0'f64 == 12;
#assert 2.0'f64 + 10's8 == 12;

#assert 10's16 + 2.0'f32 == 12;
#assert 2.0'f32 + 10's16 == 12;
#assert 10's16 + 2.0'f64 == 12;
#assert 2.0'f64 + 10's16 == 12;

#assert 10's32 + 2.0'f32 == 12;
#assert 2.0'f32 + 10's32 == 12;
#assert 10's32 + 2.0'f64 == 12;
#assert 2.0'f64 + 10's32 == 12;

#assert 10's64 + 2.0'f32 == 12;
#assert 2.0'f32 + 10's64 == 12;
#assert 10's64 + 2.0'f64 == 12;
#assert 2.0'f64 + 10's64 == 12;

#assert 1.5'f64 == 1.5'f32;
#assert 1.5'f32 == 1.5'f64;
#assert 1.5'f64 == 1.5;
#assert 1.5'f32 == 1.5;