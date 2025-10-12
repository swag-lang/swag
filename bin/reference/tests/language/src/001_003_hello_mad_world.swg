/**
# 'Hello World' Examples

Let's dig deeper into the classic 'hello world' example. The simplest version
requires no external dependencies, such as the [Swag standard modules](std.php).

'#main' is the **program entry point**, a special compiler function (its name starts
with '#'). It must be defined only once in a native executable.

'@print' is an **intrinsic**, a built-in function (its name starts with '@').
All intrinsics are part of the [compiler runtime](std/swag.runtime.php), which
is bundled with the compiler.

```swag
#main
{
    @print("Hello mad world!\n")
}
````

*/

/**
Next, a version using the 'Core.Console.print' function from the
[Std.Core](std.core.html) module. The 'Std.Core' module must be imported before use,
but this example keeps things simple.

```swag
#main
{
    Core.Console.print("Hello mad world!", "\n")
    Core.Console.printf("%\n", "Hello mad world again!")
}
```

*/

/**
A '#run' block executes **at compile time**, allowing Swag to behave like a
**scripting language**. In the following example, the famous message is printed
by the compiler during compilation.

```swag
#run
{
    const Msg = "Hello mad world!\n"   // Creates a compiler constant of type 'string'
    Core.Console.print(Msg)            // Calls 'Console.print' at compile time
}
```

*/

/**
A version that calls a **nested function** at compile time to initialize
the string constant to print.

```swag
// Brings the 'Core' namespace into scope to avoid repetition
using Core

#main
{
    #[Swag.ConstExpr]
    func nestedFunc() => "Hello mad world!\n"   // Short function syntax

    // 'nestedFunc()' can be called at compile time because it is marked with
    // the 'Swag.ConstExpr' attribute.
    const Msg = nestedFunc()
    Console.print(Msg)
}
```

*/

/**
A version that generates code dynamically using **meta programming**.

```swag
using Core

#main
{
    const Msg = "Hello mad world!\n"

    // The result of a '#ast' block is a string compiled in place.
    // This is equivalent to calling 'Console.print(Msg)' directly.
    #ast
    {
        var sb = StrConv.StringBuilder{}
        sb.appendString("Console.print(Msg)")
        return sb.toString()
    }
}
```

*/

/**
Finally, an even more elaborate example combining compile-time execution and
meta programming.

```swag
using Core

#main
{
    // '#run' forces the call to 'mySillyFunction()' at compile time
    // even though it is not marked with '#[Swag.ConstExpr]'
    const Msg = #run mySillyFunction()
    Console.print(Msg)
}

// The attribute '#[Swag.Compiler]' tells Swag that this function is for
// compile-time only, and will not be exported to the final executable or module.
#[Swag.Compiler]
func mySillyFunction()->string
{
    Console.print("Hello mad world at compile time!\n")

    // This creates a constant named 'MyConst'
    #ast
    {
        var sb = StrConv.StringBuilder{}
        sb.appendString("const MyConst = \"Hello ")
        sb.appendString("mad world ")
        sb.appendString("at runtime!\"")
        return sb.toString()
    }

    // Use and return the constant created above
    return MyConst
}
```

*/

/**
This entire sequence of meta-programming and compile-time evaluation ultimately
produces the following equivalent runtime code:

```swag
#main
{
    Core.Console.print("Hello mad world at runtime!")
}
```

*/
