#global testerror "Err0600"

#test
{
    // function pointer ↔ data pointer (if disallowed)
    func foo(x: u32) -> u32 { return x + 1 }
    let fp: func(u32)->u32 = &foo
    let p = cast([*] u8) fp             // ERROR: Invalid cast
}
