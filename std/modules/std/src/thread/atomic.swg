public namespace atomic;

#[swag.inline]
{
    func add(addr: *s8, value: s8)->s8    => @atomadd(addr, value)
    func add(addr: *s16, value: s16)->s16 => @atomadd(addr, value)
    func add(addr: *s32, value: s32)->s32 => @atomadd(addr, value)
    func add(addr: *s64, value: s64)->s64 => @atomadd(addr, value)
}