/**
A mixin in Swag is declared similarly to a function, but with the attribute `#[Swag.Mixin]`.
*/
#test
{
    #[Swag.Mixin]
    func myMixin() {}
}

/**
A mixin function is injected directly into the caller's scope, allowing it to manipulate variables or execute code as if it were part of that scope.
*/
#test
{
    #[Swag.Mixin]
    func myMixin()
    {
        a += 1
    }

    var a = 0
    myMixin() // Equivalent to writing 'a += 1' directly in the scope
    myMixin() // Again, equivalent to 'a += 1'
    @assert(a == 2)  // Verifies that 'a' has been incremented twice
}

/**
Mixins behave like functions, so you can define parameters, assign default values, and even return values.
*/
#test
{
    #[Swag.Mixin]
    func myMixin(increment: s32 = 1)
    {
        a += increment
    }

    var a = 0
    myMixin()    // Equivalent to 'a += 1', using the default value
    myMixin(2)   // Equivalent to 'a += 2', using the passed parameter
    @assert(a == 3)  // Verifies that 'a' has been incremented by 1 and 2
}

/**
A mixin can accept a special parameter of type `code`, which represents a Swag code block defined at the call site.
*/
#test
{
    #[Swag.Mixin]
    func doItTwice(what: code)
    {
        // The mixin can insert the passed code block multiple times using '#mixin'.
        #mixin what
        #mixin what
    }

    var a = 0

    // Use '#code' to define a code block that will be executed twice by the mixin.
    doItTwice(#code { a += 1; })
    @assert(a == 2)  // Verifies that 'a' was incremented twice
}

/**
When the last parameter of a mixin is of type `code`, the code can be declared in a separate statement after the mixin call.
*/
#test
{
    #[Swag.Mixin]
    func doItTwice(value: s32, what: code)
    {
        #mixin what
        #mixin what
    }

    var a = 0

    // Pass a code block using the `#code` syntax.
    doItTwice(4, #code { a += value; })

    // Alternatively, you can pass the code block directly in a more natural syntax.
    doItTwice(2)
    {
        a += value
    }

    @assert(a == 12)  // Verifies that 'a' was incremented as expected
}

/**
You can use the special name `#alias` to create a named alias for an identifier, which allows flexible variable manipulation.
*/
#test
{
    #[Swag.Mixin]
    func inc10()
    {
        #alias0 += 10  // Increment the aliased variable by 10
    }

    var a, b = 0
    inc10(|a|)  // Use 'a' as the alias
    inc10(|b|)  // Use 'b' as the alias
    @assert(a == b and b == 10)  // Verifies that both 'a' and 'b' were incremented by 10
}

#test
{
    #[Swag.Mixin]
    func setVar(value: s32)
    {
        let #alias0 = value  // Set the aliased variable to 'value'
    }

    setVar(|a| 10)  // Set 'a' to 10
    setVar(|b| 20)  // Set 'b' to 20
    @assert(a == 10)  // Verifies that 'a' was set to 10
    @assert(b == 20)  // Verifies that 'b' was set to 20
    setVar(30)  // No alias provided, so default alias '#alias0' is used
    @assert(#alias0 == 30)  // Verifies that '#alias0' was set to 30
}

/**
Mixins can declare special variables named `#mix?`. These variables receive a unique name each time the mixin is invoked, allowing the mixin to be used multiple times in the same scope without naming conflicts.
*/
#test
{
    var total: s32

    #[Swag.Mixin]
    func toScope()
    {
        var #mix0: s32 = 1  // Declare a unique variable named '#mix0'
        total += #mix0       // Add the value of '#mix0' to 'total'
    }

    toScope()  // First invocation of the mixin
    toScope()  // Second invocation
    toScope()  // Third invocation

    @assert(total == 3)  // Verifies that 'total' is the sum of all mixin invocations
}
