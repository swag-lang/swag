<p align="center">
    <img src="https://github.com/swag-lang/swag/blob/master/web/imgs/swag_logo.png">
</p>

> Work in progress ! There's no "official" release yet.

> To get the compiler latest version for Windows, go to the [release](https://github.com/swag-lang/swag/releases) page.

Swag is a native and/or interpreted system programming language made for fun because, let's be honest, C++ is now an horrible beast ! This is my third compiler (the other ones were developed for two game engines). But this one is by far the most advanced.

You can visit the official [Website](https://swag-lang.org/) or the [YouTube](https://www.youtube.com/channel/UC9dkBu1nNfJDxUML7r7QH1Q) channel. If you want to take a first look at the language, this is [here](https://www.swag-lang.org/language.php).

Note that a simple `visual studio code` [extension](https://marketplace.visualstudio.com/items?itemName=swag-lang.swag) exists for syntax coloration.

<p align="center">
    <img width="70%" height="50%" src="https://github.com/swag-lang/swag/blob/master/web/imgs/syntax.png">
</p>

### Swag is...
* **Currently in development** (since 2019), and very very far to be mature. Bugs, unexpected changes, silly decisions, do not use it to send a rocket on the moon ! It's a **toy**.
* **Low level** (i'm a C++ guy for 20+ years so i had no choice). No garbage collection like in `C#`, `Go` or `D`, no automatic pointer management like in `Swift`, no ownership like in `Rust`.
* **Statically typed** (what else), but lots of automatic type detection if you want.
* **Inspired** by a lot of things around there, like `Swift` for the syntax, `Jai` (Jonathan Blow) for the great ideas, `Go` for its simplicity, `C#` for `.NET`, `Zig` (Andrew Kelley) for the error system, `Rust` for the `impl` thing, and so on.
* Only for **Windows 10/11** and **x86_64** so far, because this is already a crazy amount of work.

### Swag is not...
* **Object oriented**, because you know what, i'm not sure this was such a good idea, after all... But with `interface`, a powerful `using` and with `UFCS` (uniform function call syntax), you can have a feeling of object oriented programming without inheritance or encapsulation.
* **Safe** at all cost. I want to be the one to make my program safe. I want to be the one to deal with memory. But Swag can help...
* **32 bits**. Only 64 bits is supported.

### Swag has...
* A **nice** and **clean syntax** (i know this is subjective). The goal is to reduce friction as much as possible. Programming should be fun.
* **Type reflection** at compile time and runtime.
* **Full compile-time execution** (your whole program can be executed by the compiler without any constraint). So Swag can also act like a scripting language.
* **Meta programming** (you can write code that writes code).
* **Interfaces** for dynamic dispatch, inspired by `Go`.
* **Modules**, compiled as separate dynamic libraries.
* **Very fast compile time** (at least in debug with the `x64` backend) thanks to heavy multithreading.
* **Fast interpretation** in script mode. Seems faster than Python or Lua for example (without JIT).
* **Simple error system**, inspired by `Zig`.
* **Generics**, for a simple usage. No template nightmare here...
* **Powerful macro/mixin** system, without the need of a specific syntax, inspired by `Jai`.
* **Unordered global declarations**, which means that the order of global declarations does not matter (they can be in any files and in whatever order).

### Swag does not have...
* **Exceptions**, because i don't like them.
* **Header files**, but who does, nowadays ?
* **Mandatory semicolons**, yeah...
* **Tagged unions**, **bitfields**, **inline assembly**..., but who knows...

# Example

Here is a **screen capture software** fully written in Swag with the help of the [Swag standard modules](https://www.swag-lang.org/std.php) (`core`, `pixel`, `gui`...). It could be compiled as a native executable (fast) or just interpreted as a script (less... fast).

<p align="center">
    <img width="70%" height="50%" src="https://github.com/swag-lang/swag/blob/master/web/imgs/captme.png">
</p>

Code is [there](https://github.com/swag-lang/swag/blob/master/bin/examples/modules/captme/src).

# For the braves

* [Start](https://swag-lang.org/getting-started.php) your first project, compile it, run it.
* Use the compiler as a [script](https://swag-lang.org/swag-as-script.php) interpreter.
* [Contribute](https://swag-lang.org/contribute-tests.php) to the compiler **test suite**, this will help (a lot).

# For the very braves

* [How to build](https://swag-lang.org/how-to-build-swag.php) the compiler with the full source tree.
