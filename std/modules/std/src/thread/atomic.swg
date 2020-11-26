public namespace atomic;

#[swag.inline]
{
    func add(addr: *s8,  value: s8)->s8   => @atomadd(addr, value)
    func add(addr: *s16, value: s16)->s16 => @atomadd(addr, value)
    func add(addr: *s32, value: s32)->s32 => @atomadd(addr, value)
    func add(addr: *s64, value: s64)->s64 => @atomadd(addr, value)

    func and(addr: *u8,  value: u8)->u8   => @atomand(addr, value)
    func and(addr: *u16, value: u16)->u16 => @atomand(addr, value)
    func and(addr: *u32, value: u32)->u32 => @atomand(addr, value)
    func and(addr: *u64, value: u64)->u64 => @atomand(addr, value)

    func or(addr: *u8,  value: u8)->u8   => @atomor(addr, value)
    func or(addr: *u16, value: u16)->u16 => @atomor(addr, value)
    func or(addr: *u32, value: u32)->u32 => @atomor(addr, value)
    func or(addr: *u64, value: u64)->u64 => @atomor(addr, value)

    func xor(addr: *u8,  value: u8)->u8   => @atomxor(addr, value)
    func xor(addr: *u16, value: u16)->u16 => @atomxor(addr, value)
    func xor(addr: *u32, value: u32)->u32 => @atomxor(addr, value)
    func xor(addr: *u64, value: u64)->u64 => @atomxor(addr, value)

    func exchange(addr: *s8,  exchangeWith: s8)->s8   => @atomxchg(addr, exchangeWith)
    func exchange(addr: *s16, exchangeWith: s16)->s16 => @atomxchg(addr, exchangeWith)
    func exchange(addr: *s32, exchangeWith: s32)->s32 => @atomxchg(addr, exchangeWith)
    func exchange(addr: *s64, exchangeWith: s64)->s64 => @atomxchg(addr, exchangeWith)

    func compareExchange(addr: *s8,  compareTo, exchangeWith: s8)->s8   => @atomcmpxchg(addr, compareTo, exchangeWith)
    func compareExchange(addr: *s16, compareTo, exchangeWith: s16)->s16 => @atomcmpxchg(addr, compareTo, exchangeWith)
    func compareExchange(addr: *s32, compareTo, exchangeWith: s32)->s32 => @atomcmpxchg(addr, compareTo, exchangeWith)
    func compareExchange(addr: *s64, compareTo, exchangeWith: s64)->s64 => @atomcmpxchg(addr, compareTo, exchangeWith)
}