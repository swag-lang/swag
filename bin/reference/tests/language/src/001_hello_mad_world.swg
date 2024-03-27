/**
Let's start with the most simple version of the "hello world" example. This is a version that does not require external dependencies like the [Swag standard modules](std.php).

`#main` is the **program entry point**, a special compiler function (that's why the name starts with `#`). It must be defined only once for a native executable.
`@print` is an **intrinsic**, a special built-in function (that's why the name starts with `@`).

All intrinsics are part of the [compiler runtime](std/swag.runtime.php) which comes with the compiler.
```swag
#main
{
    @print("Hello mad world !\n")
}
```
*/

/**
Next, a version that this time uses the `Core.Console.print` function in the [Std.Core](std.core.html) module. The `Std.Core` module would have to be imported in order to be used, but let's keep it simple.
```swag
#main
{
    Core.Console.print("Hello mad world !", "\n")
    Core.Console.printf("%\n", "Hello mad world again !")
}
```
*/

/**
A `#run` block is executed at **compile time**, and can make Swag behaves like a kind of a **scripting language**.
So in the following example, the famous message will be printed by the compiler during compilation.
```swag
#run
{
    const Msg = "Hello mad world !\n"   // Creates a compiler constant of type 'string'
    Core.Console.print(Msg)             // And call 'Console.print' at compile time
}
```
*/

/**
A version that calls a **nested function** at compile time (only) to initialize the string constant to print.
```swag
// Brings the 'Core' namespace into scope, to avoid repeating it again and again
using Core

#main
{
    #[Swag.ConstExpr]
    func nestedFunc() => "Hello mad world !\n"   // Function short syntax

    // nestedFunc() can be called at compile time because it is marked with
    // the 'Swag.ConstExpr' attribute.
    const Msg = nestedFunc()
    Console.print(Msg)
}
```
*/

/**
Now a stupid version that generates the code to do the print thanks to **meta programming**.
```swag
using Core

#main
{
    const Msg = "Hello mad world !\n"

    // The result of an '#ast' block is a string that will be compiled in place.
    // So this whole thing is equivalent to a simple 'Console.print(Msg)'.
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
And finally let's be more and more crazy.
```swag
using Core

#main
{
    // #run will force the call of mySillyFunction() at compile time even if it's not marked
    // with #[Swag.ConstExpr]
    const Msg = #run mySillyFunction()
    Console.print(Msg)
}

// The attribute #[Swag.Compiler] tells Swag that this is a compile time function only.
// So this function will not be exported to the final executable or module.
#[Swag.Compiler]
func mySillyFunction()->string
{
    Console.print("Hello mad world at compile time !\n")

    // This creates a constant named 'MyConst'
    #ast
    {
        var sb = StrConv.StringBuilder{}
        sb.appendString("const MyConst = \"Hello ")
        sb.appendString("mad world ")
        sb.appendString("at runtime !\"")
        return sb.toString()
    }

    return MyConst
}
```
*/

/**
This whole piece of code is equivalent to...
```swag
#main
{
    Core.Console.print("Hello mad world at runtime !")
}
```
*/
