//#global testerror

var G: s32 = 666

public func titi2669() => 666

public func toto2669(a: s32 = titi2669())
{
    @assert(a == 666)
}

#[swag.inline]
public func tata2669()
{
    x := @{1, 2}
    toto2669()
}