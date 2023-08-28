/**
---
<div style="background-color:Black" align="center">
    <p style="padding-top:30px;"> <img src="imgs/swag_logo.png" width=50%> </p>
    <p style="color:white;font-size:40px;line-height:1.3em;"> <b>SWAG</b> Programming Language </p>
    <div style="margin-top:50px; padding-bottom:40px; color:White; font-size:20px;">
        <p><a href="getting-started.php" style="color:#F7F900; text-decoration:none;">Getting Started</a></p>
        <p><a href="https://github.com/swag-lang/swag/releases" style="color:#F7F900; text-decoration:none;">Download the Compiler</a> </p>
    </div>
</div>

<div style="display:flex;flex-wrap:wrap;margin-bottom:30px;">
    <div style="flex:200px; padding-left:30px; padding-right:30px;">
        <h2>A sophisticated toy</h2>
        <p>Swag is a system programming language made for fun because, let's be honest, C++ is now an horrible and ugly beast !</p>
        This is my third compiler (the other ones were developed for AAA game engines), but that one is by far the most advanced.
    </div>
    <div style="flex:200px; padding-left:30px; padding-right:30px;">
        <h2>Native or interpreted</h2>
        <p>The Swag compiler can generate fast <b>native</b> code, or act as an interpreter for a <b>scripting</b> language.</p>
        Imagine C++, but where everything could be <i>constexpr</i>.
    </div>
    <div style="flex:200px; padding-left:30px; padding-right:30px;">
        <h2>Modern</h2>
        <p>Swag has <b>type reflection</b> at both runtime and compile time, <b>meta programmation</b>, <b>generics</b>, a powerful <b>macro system</b>...</p>
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

```
public mtd match(str: string)->bool
{
    if dfaNodes.isEmpty()
        return false

    var stack1, stack2: Array'(*DfaNode)
    stack1.add(dfaNodes[0])

    var pstack1 = &stack1
    var pstack2 = &stack2

    var cpt = 0
    var srcPtr = @dataof(str)
    let srcLength = @countof(str)

    while cpt < srcLength and !pstack1.isEmpty()
    {
        var (c, eat) = Utf8.decodeRune(@mkslice(srcPtr, srcLength - cpt))
        if ignoreCase
            c = Unicode.toUpper(c)

        var ok = false
        while !pstack1.isEmpty()
        {
            let curState = pstack1.popBack()
            if dfaMatch(pstack2, c, curState)
                ok = true
        }

        if ok
            cpt, srcPtr += eat

        swap(&pstack1, &pstack2)
    }

    if cpt != srcLength
        return false

    while !pstack1.isEmpty()
    {
        let curState = pstack1.popBack()
        if curState.terminal
            return true
    }

    return false
}
```

> There's **no official** release yet.
>
> Everything (including this web site) is still under construction...
*/