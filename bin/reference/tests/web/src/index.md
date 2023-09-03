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
        <p>Swag has <b>type reflection</b> at both runtime and compile time, <b>meta programming</b>, <b>generics</b>, a powerful <b>macro system</b>...</p>
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
> We haven't officially launched anything yet! Everything, even this website, is still a work in progress. So, the rules of the game can change anytime until we hit version 1.0.0. Let's keep the fun going!

```swag
func loadAssets(wnd: *Wnd) throw
{
    let render = &wnd.getApp().renderer

    var dataPath: String = Path.getDirectoryName(#location.fileName)
    dataPath = Path.combine(dataPath, "datas")
    dataPath = Path.combine(dataPath, "flappy")

    g_BirdTexture[0] = render.addImage(Path.combine(dataPath, "yellowbird-upflap.png"))
    g_BirdTexture[1] = render.addImage(Path.combine(dataPath, "yellowbird-midflap.png"))
    g_BirdTexture[2] = render.addImage(Path.combine(dataPath, "yellowbird-downflap.png"))
    g_OverTexture    = render.addImage(Path.combine(dataPath, "gameover.png"))
    g_BaseTexture    = render.addImage(Path.combine(dataPath, "base.png"))
    g_BackTexture    = render.addImage(Path.combine(dataPath, "background-day.png"))
    g_MsgTexture     = render.addImage(Path.combine(dataPath, "message.png"))

    var img = Image.load(Path.combine(dataPath, "pipe-green.png"))
    g_PipeTextureD = render.addImage(img)
    img.flip()
    g_PipeTextureU = render.addImage(img)

    g_Font = Font.create(Path.combine(dataPath, "FlappyBirdy.ttf"), 50)
}
```

Swag is **open source** and released under the [MIT license](https://github.com/swag-lang/swag/blob/master/LICENCE). You will find the compiler source code on [GitHub](https://github.com/swag-lang/swag). You can also visit the [YouTube](https://www.youtube.com/channel/UC9dkBu1nNfJDxUML7r7QH1Q) channel to see some little coding sessions.

---
<div style="display:flex; flex-wrap:space-between; height: 200px; ">
    <iframe style="flex:200px; padding:10px;" src="https://www.youtube.com/embed/Il0UuJCXTWI" title="Swag Live Coding - The Flappy Bird Game (silent)" frameborder="0" allowfullscreen></iframe>
    <iframe style="flex:200px; padding:10px;" src="https://www.youtube.com/embed/Bqr1pakewaU" title="Swag Live Coding - The Pacman Game (silent)" frameborder="0" allowfullscreen></iframe>
    <iframe style="flex:200px; padding:10px;" src="https://www.youtube.com/embed/f2rIXoH6H38" title="Swag Live Coding: The 2048 Game (silent)" frameborder="0" allowfullscreen></iframe>
</div>
---

> NOTE:
> The pages of this web site and all the related documentations have been generated with Swag, as the tool can also produce HTML based on markdown files and source code.