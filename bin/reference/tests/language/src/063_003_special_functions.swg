/**
A struct can have special operations in the `impl` block. This operations are predefined, and known by the compiler.This is the way to go to **overload operators** for example.
*/

struct Struct
{
    x, y: s32
}

typealias OneType      = bool
typealias AnotherType  = s32
typealias WhateverType = bool

impl Struct
{
    // Called each time a variable needs to be dropped
    // This is the destructor in C++
    func opDrop(using self) {}

    // Called after a raw copy operation from one value to another
    func opPostCopy(using self) {}

    // Called after a move semantic operation from one value to another. We'll see that later.
    func opPostMove(using self) {}

    // Get value by slice [low..up]
    // Must return a string or a slice
    func opSlice(using self, low, up: u64)->string { return "true"; }

    // Get value by index
    func opIndex(using self, index: OneType)->WhateverType { return true; }

    // Called by @countof
    // Use in a 'loop' block for example
    func opCount(using self)->u64          { return 0; }
    // Called by @dataof
    func opData(using self)->*WhateverType  { return null; }

    // Called for explicit/implicit casting between struct value and return type
    // Can be overloaded by a different return type
    // Example: var x = cast(OneType) v
    #[Swag.Overload]
    func opCast(using self)->OneType      { return true; }
    #[Swag.Overload]
    func opCast(using self)->AnotherType  { return 0; }

    // Called to compare the struct value with something else
    // Can be overloaded
    // Returns true if they are equal, otherwise returns false
    // Called by '==', '!='
    #[Swag.Overload]
    func opEquals(using self, other: OneType)->bool      { return false; }
    #[Swag.Overload]
    func opEquals(using self, other: AnotherType)->bool  { return false; }

    // Called to compare the struct value with something else
    // Can be overloaded
    // Returns -1, 0 or 1
    // Called by '<', '>', '<=', '>=', '<=>'
    #[Swag.Overload]
    func opCmp(using self, other: OneType)->s32      { return 0; }
    #[Swag.Overload]
    func opCmp(using self, other: AnotherType)->s32  { return 0; }

    // Affect struct with another value
    // Can be overloaded
    // Called by '='
    #[Swag.Overload]
    func opAffect(using self, other: OneType) {}
    #[Swag.Overload]
    func opAffect(using self, other: AnotherType) {}

    // Affect struct with a literal value with a specified suffix
    // Generic function, can be overloaded
    // Called by '='
    #[Swag.Overload]
    func(suffix: string) opAffectSuffix(using self, value: OneType) {}
    #[Swag.Overload]
    func(suffix: string) opAffectSuffix(using self, value: AnotherType) {}

    // Affect struct with another value at a given index
    // Can be overloaded
    // Called by '[] ='
    #[Swag.Overload]
    func opIndexAffect(using self, index: OneType, value: OneType) {}
    #[Swag.Overload]
    func opIndexAffect(using self, index: OneType, value: AnotherType) {}

    // Binary operator. 'op' generic argument contains the operator string
    // Generic function, can be overloaded
    // Called by '+', '-', '*', '/', '%', '|', '&', '^', '<<', '>>'
    #[Swag.Overload]
    func(op: string) opBinary(using self, other: OneType)->Self     { return {1, 2}; }
    #[Swag.Overload]
    func(op: string) opBinary(using self, other: AnotherType)->Self { return {1, 2}; }

    // Unary operator. 'op' generic argument contains the operator string (see below)
    // Generic function
    // Called by '!', '-', '~'
    func(op: string) opUnary(using self)->Self { return {1, 2}; }

    // Affect operator. 'op' generic argument contains the operator string (see below)
    // Generic function, can be overloaded
    // Called by '+=', '-=', '*=', '/=', '%=', '|=', '&=', '^=', '<<=', '>>='
    #[Swag.Overload]
    func(op: string) opAssign(using self, other: OneType) {}
    #[Swag.Overload]
    func(op: string) opAssign(using self, other: AnotherType)  {}

    // Affect operator. 'op' generic argument contains the operator string (see below)
    // Generic function, can be overloaded
    // Called by '+=', '-=', '*=', '/=', '%=', '|=', '&=', '^=', '<<=', '>>='
    #[Swag.Overload]
    func(op: string) opIndexAssign(using self, index: OneType, value: OneType) {}
    #[Swag.Overload]
    func(op: string) opIndexAssign(using self, index: OneType, value: AnotherType) {}

    // Called by a 'visit' block
    // Can have multiple versions, by adding a name after 'opVisit'
    // This is the way to go for iterators
    #[Swag.Macro]
    {
        func(ptr: bool, back: bool) opVisit(using self, stmt: code) {}
        func(ptr: bool, back: bool) opVisitWhatever(using self, stmt: code) {}
        func(ptr: bool, back: bool) opVisitAnother(using self, stmt: code) {}
    }
}