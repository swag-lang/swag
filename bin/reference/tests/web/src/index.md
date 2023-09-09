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
        <h2>A sophisticated toy</h2>
        <p>Swag is a <b>system</b> programming language made for fun because, let's be honest, C++ is now an horrible and ugly beast !</p>
        This is my third compiler (the other ones were developed for AAA <b>game engines</b>), but that one is by far the most advanced.
    </div>
    <div style="flex:200px; padding-left:30px; padding-right:30px;">
        <h2>Native or interpreted</h2>
        <p>The Swag compiler can generate fast <b>native</b> code with a custom x64 backend or with LLVM.
        It can also act as an interpreter for a <b>scripting</b> language.</p>
        Imagine C++, but where everything could be <i>constexpr</i>.
    </div>
    <div style="flex:200px; padding-left:30px; padding-right:30px;">
        <h2>Modern</h2>
        <p>Swag has full <b>compile-time</b> evaluation, <b>type reflection</b> at both runtime and compile time, <b>meta programming</b>, <b>generics</b>, a powerful <b>macro system</b>...</p>
        Enjoy.
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
    <p>To take a look at an explained implementation of the <b>Flappy Bird</b> game in one single little script file.</p>
    <div class="round-button">
        <a href="flappy.php" class="no-decoration">Flappy Bird</a>
    </div>
</div>
---

> WARNING:
> We haven't officially launched anything yet! Everything, even this website, is still a **work in progress**. So, the rules of the game can change anytime until we hit version 1.0.0. Let's keep the fun going!

```swag
// The 'IsSet' generic struct is generated as a mirror of the user input struct.
// Each field has the same name as the original one, but with a 'bool' type.
// It will indicate, after the command line parsing, that the corresponding argument has been
// specified or not by the user.
struct(T) IsSet
{
    #ast
    {
        var str = StrConv.StringBuilder{}
        let typeof = @typeof(T)
        visit f: typeof.fields
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
            loop i: arr
                arr[i] = 1 << cast(u32) i
            return arr
        }

    #assert PowerOfTwo[0] == 1
    #assert PowerOfTwo[1] == 2
    #assert PowerOfTwo[2] == 4
    #assert PowerOfTwo[3] == 8
}
```

Swag is **open source** and released under the [MIT license](https://github.com/swag-lang/swag/blob/master/LICENCE). You will find the compiler source code on [GitHub](https://github.com/swag-lang/swag). You can also visit the [YouTube](https://www.youtube.com/channel/UC9dkBu1nNfJDxUML7r7QH1Q) channel to see some little coding sessions.

> WARNING:
> The language evolves, and those videos can show out of date syntax. But the corresponding scripts on GitHub are always up to date and running.

---
<div style="display:flex; flex-wrap:space-between; height: 200px; ">
    <iframe style="flex:200px; padding:10px;" src="https://www.youtube.com/embed/Il0UuJCXTWI" title="Swag Live Coding - The Flappy Bird Game (silent)" frameborder="0" allowfullscreen></iframe>
    <iframe style="flex:200px; padding:10px;" src="https://www.youtube.com/embed/Bqr1pakewaU" title="Swag Live Coding - The Pacman Game (silent)" frameborder="0" allowfullscreen></iframe>
    <iframe style="flex:200px; padding:10px;" src="https://www.youtube.com/embed/f2rIXoH6H38" title="Swag Live Coding: The 2048 Game (silent)" frameborder="0" allowfullscreen></iframe>
</div>
---

> NOTE:
> The pages of this web site and all the related documentations have been generated with Swag, as the compiler can also produce HTML based on markdown files and source code.