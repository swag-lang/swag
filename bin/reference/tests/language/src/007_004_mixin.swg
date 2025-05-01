/**
# Introduction to Swag Mixins
A mixin in Swag is declared similarly to a function but with the attribute `#[Swag.Mixin]`.
Mixins allow injecting code into the caller's scope, manipulating variables, or executing code as if it were part of that scope.
This documentation provides an overview of Swag Mixins with various examples, demonstrating their flexibility and use cases.
*/

#test
{
    #[Swag.Mixin]
    func myMixin() {}  // Declaring a basic mixin with no functionality
}

/**
# Basic Example of a Mixin
A mixin function can directly modify variables in the caller's scope.
In this example, the mixin increments a variable `a` by 1 each time it is called.
*/
#test
{
    #[Swag.Mixin]
    func myMixin()
    {
        a += 1                          // Incrementing 'a' by 1
    }

    var a = 0
    myMixin()                           // Equivalent to writing 'a += 1' directly in the scope
    myMixin()                           // Again, equivalent to 'a += 1'
    @assert(a == 2)                     // Verifies that 'a' has been incremented twice
}

/**
# Mixins with Parameters
Mixins behave like functions, allowing parameters, default values, and even return values.
This example demonstrates a mixin with an `increment` parameter that defaults to 1.
*/
#test
{
    #[Swag.Mixin]
    func myMixin(increment: s32 = 1)
    {
        a += increment                  // Incrementing 'a' by the value of 'increment'
    }

    var a = 0
    myMixin()                           // Equivalent to 'a += 1', using the default value
    myMixin(2)                          // Equivalent to 'a += 2', using the passed parameter
    @assert(a == 3)                     // Verifies that 'a' has been incremented by 1 and 2
}

/**
# Mixins with Code Blocks
A mixin can accept a special parameter of type `code`, representing a Swag code block defined at the call site.
The mixin can execute this code block multiple times using the `#inject` keyword.
*/
#test
{
    #[Swag.Mixin]
    func doItTwice(what: #block void)
    {
        #inject what                      // Executing the passed code block the first time
        #inject what                      // Executing the passed code block the second time
    }

    var a = 0

    doItTwice(#code { a += 1; })         // Incrementing 'a' twice through the mixin
    @assert(a == 2)                      // Verifies that 'a' was incremented twice
}

/**
# Mixing Code Blocks in Separate Statements
When the last parameter of a mixin is of type `code`, the code can be declared in a separate statement after the mixin call.
This provides a more natural syntax for passing code blocks.
*/
#test
{
    #[Swag.Mixin]
    func doItTwice(value: s32, what: #block void)
    {
        #inject what                      // Executing the passed code block the first time
        #inject what                      // Executing the passed code block the second time
    }

    var a = 0

    doItTwice(4, #code { a += value; })  // Passing code block with direct syntax

    doItTwice(2)                         // Alternatively, pass the code block naturally
    {
        a += value                       // Incrementing 'a' by 'value' twice
    }

    @assert(a == 12)                     // Verifies that 'a' was incremented as expected
}

/**
# Creating Aliases with Mixins
You can use the special name `#alias` to create a named alias for an identifier.
This enables flexible manipulation of variables through mixins.
*/
#test
{
    #[Swag.Mixin]
    func inc10()
    {
        #alias0 += 10                    // Incrementing the aliased variable by 10
    }

    var a, b = 0
    inc10(|a|)                           // Use 'a' as the alias
    inc10(|b|)                           // Use 'b' as the alias
    @assert(a == b and b == 10)          // Verifies that both 'a' and 'b' were incremented by 10
}

#test
{
    #[Swag.Mixin]
    func setVar(value: s32)
    {
        let #alias0 = value              // Setting the aliased variable to 'value'
    }

    setVar(|a| 10)                       // Set 'a' to 10
    setVar(|b| 20)                       // Set 'b' to 20
    @assert(a == 10)                     // Verifies that 'a' was set to 10
    @assert(b == 20)                     // Verifies that 'b' was set to 20
    setVar(30)                           // No alias provided, so default alias '#alias0' is used
    @assert(#alias0 == 30)               // Verifies that '#alias0' was set to 30
}

/**
# Unique Variable Names with `#mix?`
Mixins can declare special variables named `#mix?`.
These variables receive a unique name each time the mixin is invoked, preventing naming conflicts and allowing multiple mixin invocations in the same scope.
*/
#test
{
    var total: s32

    #[Swag.Mixin]
    func toScope()
    {
        var #mix0: s32 = 1               // Declaring a unique variable named '#mix0'
        total += #mix0                   // Adding the value of '#mix0' to 'total'
    }

    toScope()                            // First invocation of the mixin
    toScope()                            // Second invocation
    toScope()                            // Third invocation

    @assert(total == 3)                  // Verifies that 'total' is the sum of all mixin invocations
}
