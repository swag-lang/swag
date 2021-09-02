![Swag logo](docs/swag_logo.png)

Swag is a programming language made for fun because, let's be honest, **C++** is now an **horrible and ugly beast** ! This is my fourth compiler (the other ones were developed for AAA game engines), but that one is by far the most advanced.

It's a toy, but it's a toy which is now more advanced than expected.

### Swag is...
* **Currently in development** (since 2019), and very very far to be mature. Bugs, unexpected changes, do not use it to send a rocket on the moon !
* **Low level** (i'm a C++ guy for 20+ years so i had no choice). No garbage collection like in C#, Go or D, no automatic pointer management like in Swift, no weird ownership like in Rust (i don't like Rust).
* **Statically typed** (what else), but lots of automatic type detection if you want to.
* **Inspired** by a lot of things around there, like **Swift** for the syntax, **Jai** (by Jonathan Blow) for the great ideas, **Go** for it's simplicity, **C#** for .NET, and **Rust** mostly for the things to avoid (i don't like Rust).
* Only for **Windows 10** and **x86_64** so far, because this is already a lot of work.

### Swag is not...
* **Object oriented**, because you know what, this was not a good idea, after all... But with a powerful *using* and with *UFCS* (universal function call system), you can have a feeling of object oriented programming without crap like inheritance or encapsulation.
* **Safe** at all cost. You should be the one to make your program safe.

### Swag has...
* A **nice** and **clean syntax** (i know this is subjective). The goal is to reduce friction as much as possible. Programming should be fun.
* **Type reflection** at compile time and runtime.
* **Full compile time execution** (your whole program can be executed by the compiler without any constraint). So Swag can also act like a scripting language.
* **Meta programming** (you can write code that writes code).
* **Interfaces** for dynamic dispatch, inspired by **Go**.
* **Modules**, compiled as separate dynamic libraries.
* **Fast compile time** (at least in debug with the x64 backend) thanks to multithreading.
* **Simple error system**, inspired from **Zig** (by Andrew Kelley).
* **Generics**, for a simple usage. No template nightmare here...
* **Powerful macro/mixin** system, without the need of a specific syntax, inspired from **Jai**.
* **Unordered global declarations**, which means that the order of global declarations does not matter (they can be in any files and in whatever order).

### Swag does not have...
* **Exceptions**, because this is bad. This is really bad.
* **Header files**, but who does, nowadays ?
* **Mandatory semicolons**, yeah...
* **Tagged unions**, **bitfields**, but who knows...

# Hello mad world !

Here without any additional library, by simply using intrinsic `@print`.
``` csharp
#main
{
    @print("Hello mad world !\n")
}
```
A version that uses the standard *core* module.
``` swag
#main
{
    using Core
    Console.print("Hello mad world !, "\n")
    Console.printf("%\n", "Hello mad world again !")
}
```
A *#run* block is executed at compile time, so the famous message will be printed by the compiler.
``` csharp
#run
{
    const Msg = "Hello mad world !\n"
    Core.Console.print(Msg)
}
```
A stupid version that generates the code to do the print.
``` csharp
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
A version that calls a nested function at compile time (only) to initialize the string constant to print.
``` csharp
using Core

#main
{
    #[Swag.ConstExpr]
    func nestedFunc() => "Hello mad world !\n"

    const Msg = nestedFunc()
    Console.print(Msg)
}
```
More and more crazy.
``` csharp
using Core

#main
{
    const Msg = #run mySillyFunction()
    Console.print(Msg)
}

#[Swag.Compiler]
func mySillyFunction()->string
{
    Console.print("Hello mad world at compile time !\n")

    #ast
    {
        sb := StrConv.StringBuilder{}
        sb.appendString("const Msg = \"Hello ")
        sb.appendString("mad world ")
        sb.appendString("at runtime !\"")
        return sb.toString()
    }

    return Msg
}
```

# For the braves

* [Start](docs/getting_started.md) your first project, compile it, run it !

# For the very braves

* [How to build](docs/how_to_build_swag.md) the compiler with the full source tree.
