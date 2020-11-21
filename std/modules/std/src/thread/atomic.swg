public namespace atomic;

#[swag.inline]
{
    func add(addr: *s8,  value: s8)->s8   => @atomadd(addr, value)
    func add(addr: *s16, value: s16)->s16 => @atomadd(addr, value)
    func add(addr: *s32, value: s32)->s32 => @atomadd(addr, value)
    func add(addr: *s64, value: s64)->s64 => @atomadd(addr, value)

    func and(addr: *s8,  value: s8)->s8   => @atomand(addr, value)
    func and(addr: *s16, value: s16)->s16 => @atomand(addr, value)
    func and(addr: *s32, value: s32)->s32 => @atomand(addr, value)
    func and(addr: *s64, value: s64)->s64 => @atomand(addr, value)

    func or(addr: *s8,  value: s8)->s8   => @atomor(addr, value)
    func or(addr: *s16, value: s16)->s16 => @atomor(addr, value)
    func or(addr: *s32, value: s32)->s32 => @atomor(addr, value)
    func or(addr: *s64, value: s64)->s64 => @atomor(addr, value)

    func xor(addr: *s8,  value: s8)->s8   => @atomxor(addr, value)
    func xor(addr: *s16, value: s16)->s16 => @atomxor(addr, value)
    func xor(addr: *s32, value: s32)->s32 => @atomxor(addr, value)
    func xor(addr: *s64, value: s64)->s64 => @atomxor(addr, value)

    func compareExchange(addr: *s8,  compareTo, exchangeWith: s8)->s8   => @atomcmpxchg(addr, compareTo, exchangeWith)
    func compareExchange(addr: *s16, compareTo, exchangeWith: s16)->s16 => @atomcmpxchg(addr, compareTo, exchangeWith)
    func compareExchange(addr: *s32, compareTo, exchangeWith: s32)->s32 => @atomcmpxchg(addr, compareTo, exchangeWith)
    func compareExchange(addr: *s64, compareTo, exchangeWith: s64)->s64 => @atomcmpxchg(addr, compareTo, exchangeWith)
}