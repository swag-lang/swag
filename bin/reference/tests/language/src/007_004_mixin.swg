/**
# Introduction to Swag Mixins
A mixin in Swag is declared similarly to a function but with the attribute '#[Swag.Mixin]'.
Mixins inject code into the caller's scope, manipulate variables, or execute as if written in that scope.
This file provides a clear set of examples and tests.
*/

#test
{
    #[Swag.Mixin]
    func myMixin()
    {
        // Basic empty mixin
    }
}

/**
# Basic Example of a Mixin
A mixin can directly modify variables in the caller's scope.
This example increments 'a' by 1 each time it is called.
*/
#test
{
    #[Swag.Mixin]
    func myMixin()
    {
        a += 1
    }

    var a = 0
    myMixin() // Equivalent to writing 'a += 1' directly here
    myMixin()
    @assert(a == 2)
}

/**
# Mixins with Parameters
Mixins behave like functions: they can take parameters, default values, and return values.
Here, 'increment' defaults to 1.
*/
#test
{
    #[Swag.Mixin]
    func myMixin(increment: s32 = 1)
    {
        a += increment
    }

    var a = 0
    myMixin() // Uses default: 'a += 1'
    myMixin(2) // Uses provided value: 'a += 2'
    @assert(a == 3)
}

/**
# Mixins with Code Blocks
A mixin can accept a parameter of type 'code', representing a Swag code block defined at the call site.
The mixin can execute this block multiple times using '#inject'.
*/
#test
{
    #[Swag.Mixin]
    func doItTwice(what: #code void)
    {
        #inject(what)
        // first execution
        #inject(what)
        // second execution
    }

    var a = 0
    doItTwice(#code { a += 1; })
    @assert(a == 2)
}

/**
# Passing Code Blocks in Separate Statements
When the last parameter is of type 'code', the code can be provided in a separate block after the call.
*/
#test
{
    #[Swag.Mixin]
    func doItTwice(value: s32, what: #code void)
    {
        #inject(what)
        #inject(what)
    }

    var a = 0

    // Inline code argument
    doItTwice(4, #code { a += value; })

    // Separate trailing block
    doItTwice(2)
    {
        a += value
    }

    @assert(a == 12)
}

/**
# Creating Aliases with Mixins
Use the special name '#alias' to create a named alias for an identifier.
This enables flexible manipulation of variables through mixins.
*/
#test
{
    #[Swag.Mixin]
    func inc10()
    {
        #alias0 += 10
    }

    var a, b = 0
    inc10(|a|) // use 'a' as the alias
    inc10(|b|) // use 'b' as the alias
    @assert(a == b and b == 10)
}

#test
{
    #[Swag.Mixin]
    func setVar(value: s32)
    {
        let #alias0 = value
    }

    setVar(|a| 10)
    setVar(|b| 20)
    @assert(a == 10)
    @assert(b == 20)

    // No explicit alias: default '#alias0' is used
    setVar(30)
    @assert(#alias0 == 30)
}

/**
# Unique Variable Names with '#uniq?'
Mixins can declare special variables named '#uniq?'.
Each invocation receives a unique symbol, avoiding naming conflicts and allowing multiple calls in the same scope.
*/
#test
{
    var total: s32

    #[Swag.Mixin]
    func toScope()
    {
        var #uniq0: s32 = 1
        total += #uniq0
    }

    toScope()
    toScope()
    toScope()

    @assert(total == 3)
}
