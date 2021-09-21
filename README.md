![Swag logo](docs/swag_logo.png)

Swag is a programming language made for fun because, let's be honest, **C++** is now an **horrible and ugly beast** ! This is my third compiler (the other ones were developed for AAA game engines), but that one is by far the most advanced.

For now it's a **toy**, but it's a toy which is more advanced than expected.

### Swag is...
* **Currently in development** (since 2019), and very very far to be mature. Bugs, unexpected changes, silly decisions, do not use it to send a rocket on the moon !
* **Low level** (i'm a C++ guy for 20+ years so i had no choice). No garbage collection like in C#, Go or D, no automatic pointer management like in Swift, no weird ownership like in Rust.
* **Statically typed** (what else), but lots of automatic type detection if you want.
* **Inspired** by a lot of things around there, like **Swift** for the syntax, **Jai** (Jonathan Blow) for the great ideas, **Go** for it's simplicity, **C#** for .NET, **Zig** (Andrew Kelley) for the error system, **Rust** for the *impl* thing, and so on.
* Only for **Windows 10** and **x86_64** so far, because this is already a lot of work.

### Swag is not...
* **Object oriented**, because you know what, this was not a good idea, after all... But with a powerful *using* and with *UFCS* (uniform function call syntax), you can have a feeling of object oriented programming without inheritance or encapsulation.
* **Safe** at all cost. You should be the one to make your program safe. You should be the one to deal with memory.
* **32 bits**. Only 64 bits compile is supported.

### Swag has...
* A **nice** and **clean syntax** (i know this is subjective). The goal is to reduce friction as much as possible. Programming should be fun.
* **Type reflection** at compile time and runtime.
* **Full compile time execution** (your whole program can be executed by the compiler without any constraint). So Swag can also act like a scripting language.
* **Meta programming** (you can write code that writes code).
* **Interfaces** for dynamic dispatch, inspired by **Go**.
* **Modules**, compiled as separate dynamic libraries.
* **Fast compile time** (at least in debug with the x64 backend) thanks to multithreading.
* **Simple error system**, inspired by **Zig**.
* **Generics**, for a simple usage. No template nightmare here...
* **Powerful macro/mixin** system, without the need of a specific syntax, inspired by **Jai**.
* **Unordered global declarations**, which means that the order of global declarations does not matter (they can be in any files and in whatever order).

### Swag does not have...
* **Exceptions**, because this is bad.
* **Header files**, but who does, nowadays ?
* **Mandatory semicolons**, yeah...
* **Tagged unions**, **bitfields**, **inline assembly**... But who knows...

# Hello mad world !

`#main` is the program entry point, a special compiler function. That's why the name starts with `#`.
`@print` is an intrinsic, a special built-in function. That's why the name starts with `@`.

``` swift
#main
{
    @print("Hello mad world !\n")
}
```
A version that uses the standard *core* module :

``` swift
#main
{
    using Core
    Console.print("Hello mad world !, "\n")
    Console.printf("%\n", "Hello mad world again !")
}
```
A *#run* block is executed at compile time, so the famous message will be printed by the compiler :

``` swift
#run
{
    const Msg = "Hello mad world !\n"
    Core.Console.print(Msg)
}
```
A stupid version that generates the code to do the print (meta programming) :

``` swift
using Core

#main
{
    const Msg = "Hello mad world !\n"

    // The result of #ast is a string that will be compiled in place
    #ast
    {
        sb := StrConv.StringBuilder{}
        sb.appendString("Console.print(Msg)")
        return sb.toString()
    }
}
```

A version that calls a nested function at compile time (only) to initialize the string constant to print :

``` swift
using Swag, Core

#main
{
    #[ConstExpr]
    func nestedFunc() => "Hello mad world !\n"

    // nestedFunc() can be called at compile time because it is marked with 'Swag.ConstExpr'
    const Msg = nestedFunc()
    Console.print(Msg)
}
```

More and more crazy :

``` swift
using Core

#main
{
    // #run will force the call of mySillyFunction() at compile time
    const Msg = #run mySillyFunction()
    Console.print(Msg)
}

// This is a compile time function only
#[Swag.Compiler]
func mySillyFunction()->string
{
    Console.print("Hello mad world at compile time !\n")

    // This creates a constant named 'MyConst'
    #ast
    {
        sb := StrConv.StringBuilder{}
        sb.appendString("const MyConst = \"Hello ")
        sb.appendString("mad world ")
        sb.appendString("at runtime !\"")
        return sb.toString()
    }

    return MyConst
}
```

# For the braves

* [Start](docs/getting_started.md) your first project, compile it, run it !

# For the very braves

* [How to build](docs/how_to_build_swag.md) the compiler with the full source tree.
