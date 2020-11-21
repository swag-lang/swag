public namespace atomic;

#[swag.inline]
{
    func add(addr: *s8, value: s8)->s8    => @atomadd(addr, value)
    func add(addr: *s16, value: s16)->s16 => @atomadd(addr, value)
    func add(addr: *s32, value: s32)->s32 => @atomadd(addr, value)
    func add(addr: *s64, value: s64)->s64 => @atomadd(addr, value)

    func and(addr: *s8, value: s8)->s8    => @atomand(addr, value)
    func and(addr: *s16, value: s16)->s16 => @atomand(addr, value)
    func and(addr: *s32, value: s32)->s32 => @atomand(addr, value)
    func and(addr: *s64, value: s64)->s64 => @atomand(addr, value)

    func or(addr: *s8, value: s8)->s8    => @atomor(addr, value)
    func or(addr: *s16, value: s16)->s16 => @atomor(addr, value)
    func or(addr: *s32, value: s32)->s32 => @atomor(addr, value)
    func or(addr: *s64, value: s64)->s64 => @atomor(addr, value)

    func xor(addr: *s8, value: s8)->s8    => @atomxor(addr, value)
    func xor(addr: *s16, value: s16)->s16 => @atomxor(addr, value)
    func xor(addr: *s32, value: s32)->s32 => @atomxor(addr, value)
    func xor(addr: *s64, value: s64)->s64 => @atomxor(addr, value)

    /*func compareExchange(addr: *s8, exchangeWith: s8, compareTo: s8)->s8     => @atomcmpxchg(addr, exchangeWith, compareTo)
    func compareExchange(addr: *s16, exchangeWith: s16, compareTo: s16)->s16 => @atomcmpxchg(addr, exchangeWith, compareTo)
    func compareExchange(addr: *s32, exchangeWith: s32, compareTo: s32)->s32 => @atomcmpxchg(addr, exchangeWith, compareTo)
    func compareExchange(addr: *s64, exchangeWith: s64, compareTo: s64)->s64 => @atomcmpxchg(addr, exchangeWith, compareTo)*/
}