#test
{
    // A mixin is declared like a function, with the attribute 'swag.mixin'
    {
        #[swag.mixin]
        func myMixin()
        {
        }
    }

    // A mixin function is inserted in the scope of the caller
    {
        #[swag.mixin]
        func myMixin()
        {
            a += 1
        }

        a := 0
        myMixin()
        myMixin()
        @assert(a == 2)
    }

    // This behaves like a function, so you can add parameters
    {
        #[swag.mixin]
        func myMixin(increment: s32 = 1)
        {
            a += increment
        }

        a := 0
        myMixin()
        myMixin(2)
        @assert(a == 3)
    }

    // A mixin accepts parameters of type 'code'
    {
        #[swag.mixin]
        func doItTwice(what: code)
        {
            #mixin what
            #mixin what
        }

        a := 0
        doItTwice(#code {a += 1;})
        @assert(a == 2)
    }

    // When the last parameter of a mixin is of type 'code', caller
    // can declare that code in a statement just after the call
    {
        #[swag.mixin]
        func doItTwice(value: s32, what: code)
        {
            #mixin what
            #mixin what
        }

        a := 0
        doItTwice(4)
        {
            a += value
        }

        doItTwice(2)
        {
            a += value
        }

        @assert(a == 12)
    }

    // You can use special type 'alias' to create a named alias
    // for an identifier
    {
        #[swag.mixin]
        func inc10(variable: alias)
        {
            variable += 10
        }

        a, b := 0
        inc10(a)
        inc10(b)
        @assert(a == b && b == 10)
    }

    // You can also use special type 'alias' and '#code' to create a named alias
    // for a variable declaration
    {
        #[swag.mixin]
        func setVar(variable: alias, value: s32)
        {
            variable := value
        }

        setVar(#code a, 10)
        setVar(#code b, 20)
        @assert(a == 10)
        @assert(b == 20)
    }
}

// A mixin function can be called in the global scope
/*#[swag.mixin]
func declare(name: alias, value: s32)
{
    const name: s32 = value
}

declare(#code myVar, 5)
declare(#code myVar1, 10)
#assert myVar == 5
#assert myVar1 == 10*/