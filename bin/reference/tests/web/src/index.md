<html>
<div style="background-color:Black; border-radius: 5px;" align="center">
    <p style="padding-top:30px;"><img src="imgs/swag_logo.png" width="50%"></p>
    <p style="color:white; font-size:40px; line-height:1.3em;"><b>SWAG</b> Programming Language</p>
    <div style="margin-top:50px; padding-bottom:20px; color:white; font-size:20px;">
        <div class="round-button"><a href="getting-started.php" style="color:#F7F900; text-decoration:none;">Getting Started</a></div>
        <div class="round-button"><a href="https://github.com/swag-lang/swag/releases" style="color:#F7F900; text-decoration:none;">Download the Compiler</a></div>
    </div>
</div>

<div style="display:flex; flex-wrap:wrap; margin-bottom:30px;">
    <div style="flex:200px; padding-left:30px; padding-right:30px;">
        <h2>A Modern Playground</h2>
        <p>Swag is a <b>systems</b> programming language crafted for fun, offering a fresh alternative to the increasingly cumbersome C++.</p>
        <p>This is my third compiler project (the previous ones powered AAA <b>game engines</b>), but Swag represents a significant leap forward in terms of capability and design.</p>
    </div>

    <div style="flex:200px; padding-left:30px; padding-right:30px;">
        <h2>Flexible and Powerful</h2>
        <p>The Swag compiler can generate high-performance <b>native</b> code using a custom x64 backend or LLVM, and it can also function as an interpreter for <b>scripting</b> purposes.</p>
        <p>Think of it as C++, but with the ability to make everything <i>constexpr</i>.</p>
    </div>

    <div style="flex:200px; padding-left:30px; padding-right:30px;">
        <h2>Cutting-Edge Features</h2>
        <p>Swag offers advanced features like full <b>compile-time</b> evaluation, <b>type reflection</b> at both runtime and compile time, <b>meta-programming</b>, <b>generics</b>, a powerful <b>macro system</b>, and much more.</p>
        <p>It's designed to be both fun and highly productive.</p>
    </div>
</div>

<div align="center">
    <div class="round-button">
        <a href="language.php" class="no-decoration">Documentation</a>
    </div>
    <div class="round-button">
        <a href="swag-as-script.php" class="no-decoration">Scripting</a>
    </div>
    <div class="round-button">
        <a href="std.php">Standard Modules</a>
    </div>
</div>
</html>
<html>
<div align="center">
    <p>Explore a detailed, single-script implementation of the <b>Flappy Bird</b> game.</p>
    <div class="round-button">
        <a href="flappy.php" class="no-decoration">Flappy Bird</a>
    </div>
</div>
</html>

> WARNING:  
> Swag is still under active development! Everything, including this website, is a **work in progress**. Features and details are subject to change until we officially reach version 1.0.0. Stay tuned and enjoy the ride!

```swag
// The 'IsSet' generic struct mirrors the user-defined struct,
// with each field converted to a 'bool' type.
// After command-line parsing, it indicates whether each argument
// was provided by the user.
struct(T) IsSet
{
    #ast
    {
        var str = StrConv.StringBuilder{}
        let typeof = #typeof(T)
        foreach f in typeof.fields
            str.appendFormat("%: bool\n", f.name)
        return str.toString()
    }
}
```

```swag
#test
{
    const N = 4
    const PowerOfTwo: [N] s32 = #run
        {
            var arr: [N] s32
            for i in arr
                arr[i] = 1 << cast(u32) i
            return arr
        }

    #assert PowerOfTwo[0] == 1
    #assert PowerOfTwo[1] == 2
    #assert PowerOfTwo[2] == 4
    #assert PowerOfTwo[3] == 8
}
```

Swag is **open source** and released under the [MIT license](https://github.com/swag-lang/swag/blob/master/LICENCE). The compiler source code is available on [GitHub](https://github.com/swag-lang/swag). Check out our [YouTube](https://www.youtube.com/channel/UC9dkBu1nNfJDxUML7r7QH1Q) channel for coding sessions.

> WARNING:  
> As the language evolves, some videos might show outdated syntax. However, the corresponding scripts on GitHub are always up-to-date and functional.

<html>
<div style="display:flex; flex-wrap:wrap;">
    <iframe style="width:200px; height:200px; flex:200px; padding:10px;" src="https://www.youtube.com/embed/Il0UuJCXTWI" title="Swag Live Coding - The Flappy Bird Game (silent)" frameborder="0" allowfullscreen></iframe>
    <iframe style="width:200px; height:200px; flex:200px; padding:10px;" src="https://www.youtube.com/embed/Bqr1pakewaU" title="Swag Live Coding - The Pacman Game (silent)" frameborder="0" allowfullscreen></iframe>
    <iframe style="width:200px; height:200px; flex:200px; padding:10px;" src="https://www.youtube.com/embed/f2rIXoH6H38" title="Swag Live Coding: The 2048 Game (silent)" frameborder="0" allowfullscreen></iframe>
</div>
</html>

> NOTE:  
> This website and all related documentation were generated using Swag. The compiler can also produce HTML from markdown files and source code.
