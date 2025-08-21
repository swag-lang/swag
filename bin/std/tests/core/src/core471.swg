using Core, Core.Hash

struct MyStruct115 { x, y: s32 }

impl IHash32 for MyStruct115
{
    mtd const impl compute()->u32
    {
        return cast(u32) me.x
    }
}

struct MyStruct115_64 { x, y: s32 }

impl IHash64 for MyStruct115_64
{
    mtd const impl compute()->u64
    {
        return cast(u64) me.y
    }
}

#test
{
    let v1 = hash64("this is a string")
    @assert(v1 != 0)

    let x: MyStruct115 = {10, 20}
    let v2 = hash64(x)
    @assert(v2 == 10)

    let y: MyStruct115_64 = {10, 20}
    let v3 = hash64(y)
    @assert(v3 == 20)
}
