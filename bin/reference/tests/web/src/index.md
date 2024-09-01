---
<div style="background-color:Black; border-radius: 5px;" align="center">
    <p style="padding-top:30px;"> <img src="imgs/swag_logo.png" width=50%> </p>
    <p style="color:white;font-size:40px;line-height:1.3em;"> <b>SWAG</b> Programming Language </p>
    <div style="margin-top:50px; padding-bottom:20px; color:White; font-size:20px;">
        <div class="round-button"><a href="getting-started.php" style="color:#F7F900; text-decoration:none;">Getting Started</a></div>
        <div class="round-button"><a href="https://github.com/swag-lang/swag/releases" style="color:#F7F900; text-decoration:none;">Download the Compiler</a> </div>
    </div>
</div>

<div style="display:flex;flex-wrap:wrap;margin-bottom:30px;">
    <div style="flex:200px; padding-left:30px; padding-right:30px;">
        <h2>A Sophisticated Toy</h2>
        <p>Swag is a <b>systems</b> programming language created for fun, because, let's be honest, C++ has become a monstrous and unwieldy beast!</p>
        <p>This is my third compiler (the previous ones were developed for AAA <b>game engines</b>), and it is by far the most advanced.</p>
    </div>

    <div style="flex:200px; padding-left:30px; padding-right:30px;">
        <h2>Native or Interpreted</h2>
        <p>The Swag compiler can generate efficient <b>native</b> code using a custom x64 backend or LLVM. It can also function as an interpreter for a <b>scripting</b> language.</p>
        <p>Imagine C++, but where everything could be <i>constexpr</i>.</p>
    </div>

    <div style="flex:200px; padding-left:30px; padding-right:30px;">
        <h2>Modern</h2>
        <p>Swag offers full <b>compile-time</b> evaluation, <b>type reflection</b> at both runtime and compile time, <b>meta-programming</b>, <b>generics</b>, a powerful <b>macro system</b>, and more.</p>
        <p>Enjoy.</p>
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
---
---
<div align="center">
    <p>Take a look at a detailed implementation of the <b>Flappy Bird</b> game in a single, compact script file.</p>
    <div class="round-button">
        <a href="flappy.php" class="no-decoration">Flappy Bird</a>
    </div>
</div>
---
> WARNING:  
> We haven't officially launched anything yet! Everything, including this website, is still a **work in progress**. The rules of the game can change anytime until we hit version 1.0.0. Let's keep the fun going!

```swag
// The 'IsSet' generic struct is generated as a mirror of the user input struct.
// Each field has the same name as the original one, but with a 'bool' type.
// It indicates, after command line parsing, whether the corresponding argument has been
// specified by the user.
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

Swag is **open source** and released under the [MIT license](https://github.com/swag-lang/swag/blob/master/LICENCE). The compiler source code is available on [GitHub](https://github.com/swag-lang/swag). You can also visit our [YouTube](https://www.youtube.com/channel/UC9dkBu1nNfJDxUML7r7QH1Q) channel for coding sessions.

> WARNING:  
> The language is evolving, and some videos may show outdated syntax. However, the corresponding scripts on GitHub are always up-to-date and functional.

---

<div style="display:flex; flex-wrap:wrap;">
    <iframe style="width:200px; height:200px; flex:200px; padding:10px;" src="https://www.youtube.com/embed/Il0UuJCXTWI" title="Swag Live Coding - The Flappy Bird Game (silent)" frameborder="0" allowfullscreen></iframe>
    <iframe style="width:200px; height:200px; flex:200px; padding:10px;" src="https://www.youtube.com/embed/Bqr1pakewaU" title="Swag Live Coding - The Pacman Game (silent)" frameborder="0" allowfullscreen></iframe>
    <iframe style="width:200px; height:200px; flex:200px; padding:10px;" src="https://www.youtube.com/embed/f2rIXoH6H38" title="Swag Live Coding: The 2048 Game (silent)" frameborder="0" allowfullscreen></iframe>
</div>

---

> NOTE:  
> The pages on this website and all related documentation have been generated with Swag, as the compiler can also produce HTML from markdown files and source code.