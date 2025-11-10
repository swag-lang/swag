using Swag

public interface IHash64
{
    mtd const compute()->u64;
}

//func isStruct(t: typeinfo) => t.kind == .Struct

// Hash the given value and returns the corresponding u64
#[ConstExpr, Safety(.All, .All, false)]
public func(T) hash64(value: T)->u64
{
    /*#if T == string:
        return 0*/
    /*isStruct(T) and*/
    #if #defined(T.IHash64) do
        #ast "return 0" // ++ #typeof(T).fullname ++ ".IHash64.compute(value)"
    #else do
        return 0
}

struct MyStruct115 { x, y: s32 }

#test
{
    let x: MyStruct115 = {10, 20}
    let v2 = hash64(x)
    //@assert(v2 == 10)
}
