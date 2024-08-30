/**
A struct in Swag can define special operations within the `impl` block. These operations are predefined
and recognized by the compiler. This enables the ability to **overload operators** and customize behavior
when interacting with the struct.
*/

struct Struct
{
    x, y: s32   // Define two properties, x and y, of type s32 (signed 32-bit integer)
}

typealias OneType      = bool   // Alias for boolean type
typealias AnotherType  = s32    // Alias for signed 32-bit integer type
typealias WhateverType = bool   // Another alias for boolean type

impl Struct
{
    // Called whenever a variable of this struct is about to be destroyed (similar to a destructor in C++)
    func opDrop(using self) {}

    // Invoked after a raw copy operation has been performed from one value to another
    func opPostCopy(using self) {}

    // Called after a move semantic operation has been executed from one value to another
    func opPostMove(using self) {}

    // Access a value by slicing with the [low..up] notation. Returns a string or a slice.
    func opSlice(using self, low, up: u64)->string { return "true"; }

    // Access a value by its index. The index is of type OneType, and it returns a WhateverType.
    func opIndex(using self, index: OneType)->WhateverType { return true; }

    // Called when @countof is used, typically in a 'for' block, to return the count of elements
    func opCount(using self)->u64 { return 0; }

    // Called when @dataof is used, returns a pointer to the underlying data of type WhateverType
    func opData(using self)->*WhateverType { return null; }

    // Custom casting between the struct and another type, can be overloaded with different return types
    // Example usage: var x = cast(OneType) v
    #[Swag.Overload]
    func opCast(using self)->OneType { return true; }
    #[Swag.Overload]
    func opCast(using self)->AnotherType { return 0; }

    // Compare the struct value with another, can be overloaded. 
    // Returns true if equal, otherwise false. Used in '==', '!=' operations.
    #[Swag.Overload]
    func opEquals(using self, other: OneType)->bool { return false; }
    #[Swag.Overload]
    func opEquals(using self, other: AnotherType)->bool { return false; }

    // Compares the struct value with another, returns -1, 0, or 1. 
    // Used in comparison operations like '<', '>', '<=', '>=', '<=>'.
    #[Swag.Overload]
    func opCmp(using self, other: OneType)->s32 { return 0; }
    #[Swag.Overload]
    func opCmp(using self, other: AnotherType)->s32 { return 0; }

    // Assigns a value to the struct, can be overloaded. 
    // This is triggered by the '=' operator.
    #[Swag.Overload]
    func opAffect(using self, other: OneType) {}
    #[Swag.Overload]
    func opAffect(using self, other: AnotherType) {}

    // Assign a literal value with a specific suffix to the struct. 
    // This is a generic function and can be overloaded.
    #[Swag.Overload]
    func(suffix: string) opAffectLiteral(using self, value: OneType) {}
    #[Swag.Overload]
    func(suffix: string) opAffectLiteral(using self, value: AnotherType) {}

    // Assign a value to a specific index in the struct. 
    // Can be overloaded, used by '[] =' syntax.
    #[Swag.Overload]
    func opIndexAffect(using self, index: OneType, value: OneType) {}
    #[Swag.Overload]
    func opIndexAffect(using self, index: OneType, value: AnotherType) {}

    // Perform a binary operation, with 'op' representing the operator as a string. 
    // This is a generic function that can be overloaded.
    // Examples include '+', '-', '*', '/', '%', '|', '&', '^', '<<', '>>'
    #[Swag.Overload]
    func(op: string) opBinary(using self, other: OneType)->Self { return {1, 2}; }
    #[Swag.Overload]
    func(op: string) opBinary(using self, other: AnotherType)->Self { return {1, 2}; }

    // Perform a unary operation, with 'op' representing the operator as a string. 
    // This is a generic function.
    // Examples include '!', '-', '~'
    func(op: string) opUnary(using self)->Self { return {1, 2}; }

    // Perform an assignment operation, with 'op' representing the operator as a string. 
    // This is a generic function that can be overloaded.
    // Examples include '+=', '-=', '*=', '/=', '%=', '|=', '&=', '^=', '<<=', '>>='
    #[Swag.Overload]
    func(op: string) opAssign(using self, other: OneType) {}
    #[Swag.Overload]
    func(op: string) opAssign(using self, other: AnotherType) {}

    // Assign a value to an indexed position with an operator, 
    // 'op' represents the operator as a string. Can be overloaded.
    #[Swag.Overload]
    func(op: string) opIndexAssign(using self, index: OneType, value: OneType) {}
    #[Swag.Overload]
    func(op: string) opIndexAssign(using self, index: OneType, value: AnotherType) {}

    // Called in a 'foreach' block to iterate over the struct's elements. 
    // Multiple versions can be defined by adding a name after 'opVisit'.
    #[Swag.Macro]
    {
        func(ptr: bool, back: bool) opVisit(using self, stmt: code) {}
        func(ptr: bool, back: bool) opVisitWhatever(using self, stmt: code) {}
        func(ptr: bool, back: bool) opVisitAnother(using self, stmt: code) {}
    }
}
