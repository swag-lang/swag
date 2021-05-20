// A struct can have special operations in the 'impl' block.
// This operations are predefined, and known by the compiler.
// This is the way to go to overload operators for example.

struct Struct
{
    x, y: s32
}

alias OneType = bool
alias AnotherType = s32
alias WhateverType = bool

impl Struct
{
    // Called each time a variable needs to be dropped
    // This is the destructor in c++
    func opDrop(using self) {}

    // Called after a raw copy operation from one value to another
    func opPostCopy(using self) {}

    // Called after a,move semantic operation from one value to another
    func opPostMove(using self) {}

    // Get value by slice [low..up]
    func opSlice(using self, low, up: uint)->WhateverType { return true; }

    // Get value by index
    func opIndex(using self, index: uint)->WhateverType { return true; }

    // Called by @countof
    // Use in 'loop' block
    func opCount(using self)->uint          { return 0; }
    // Called by @dataof
    func opData(using self)->*WhateverType  { return null; }

    // Can be overloaded by a different return type
    // Called for explicit/implicit casting between struct value and return type
    // Example: x := cast(OneType) v
    func opCast(using self)->OneType      { return true; }
    func opCast(using self)->AnotherType  { return 0; }

    // Can be overloaded
    // Called to compare the struct value with something else
    // Returns true if they are equal, otherwise returns false
    // Called by '==', '!='
    func opEquals(using self, other: OneType)->bool      { return false; }
    func opEquals(using self, other: AnotherType)->bool  { return false; }

    // Can be overloaded
    // Called to compare the struct value with something else
    // Returns -1, 0 or 1
    // Called by '<', '>', '<=', '>=', '<=>'
    func opCmp(using self, other: OneType)->s32      { return 0; }
    func opCmp(using self, other: AnotherType)->s32  { return 0; }

    // Can be overloaded
    // Affect struct with another value
    // Called by '='
    func opAffect(using self, other: OneType) {}
    func opAffect(using self, other: AnotherType) {}

    // Can be overloaded
    // Affect struct with another value at a given index
    // Called by '[] ='
    func opIndexAffect(using self, index: uint, value: OneType) {}
    func opIndexAffect(using self, index: uint, value: AnotherType) {}

    // Generic function, can be overloaded
    // Binary operator. 'op' generic argument contains the operator string (see below)
    // Called by '+', '-', '*', '/', '%', '|', '&', '^', '<<', '>>'
    func(op: string) opBinary(using self, other: OneType)->Self     { return @{1, 2}; }
    func(op: string) opBinary(using self, other: AnotherType)->Self { return @{1, 2}; }

    // Generic function
    // Unary operator. 'op' generic argument contains the operator string (see below)
    // Called by '!', '-', '~'
    func(op: string) opUnary(using self)->Self { return @{1, 2}; }

    // Generic function, can be overloaded
    // Affect operator. 'op' generic argument contains the operator string (see below)
    // Called by '+=', '-=', '*=', '/=', '%=', '|=', '&=', '^=', '<<=', '>>='
    func(op: string) opAssign(using self, other: OneType) {}
    func(op: string) opAssign(using self, other: AnotherType)  {}

    // Generic function, can be overloaded
    // Affect operator. 'op' generic argument contains the operator string (see below)
    // Called by '+=', '-=', '*=', '/=', '%=', '|=', '&=', '^=', '<<=', '>>='
    func(op: string) opIndexAssign(using self, index: uint, value: OneType) {}
    func(op: string) opIndexAssign(using self, index: uint, value: AnotherType) {}

    // Called by a 'visit' block
    // Can have multiple versions, by adding a name after 'opVisit'
    #[Swag.macro]
    {
        func(ptr: bool) opVisit(using self, stmt: code) {}
        func(ptr: bool) opVisitWhatever(using self, stmt: code) {}
        func(ptr: bool) opVisitAnother(using self, stmt: code) {}
    }
}