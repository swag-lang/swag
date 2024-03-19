This is the list of all modules that come with the compiler. As they are always in sync, they are considered as **standard**. They are all part of the same workspace **std**.

You can find that workspace locally in `bin/std`, or [here](https://github.com/swag-lang/swag/tree/master/bin/std) on GitHub.

# Modules

| [std.core](std.core.php)         | Main core module, the base of everything else
| [std.pixel](std.pixel.php)       | An image and a 2D painting module
| [std.gui](std.gui.php)           | A user interface module (windows, widgets...)
| [std.audio](std.audio.php)       | An audio module to decode and play sounds
| [std.libc](std.libc.php)         | Libc wrapper

# Wrappers
Those other modules are just wrappers to external libraries.

| [std.ogl](std.ogl.php)           | Opengl wrapper
| [std.freetype](std.freetype.php) | Freetype wrapper
| [std.win32](std.win32.php)       | Windows 'win32' wrapper (kernel32, user32...)
| [std.gdi32](std.gdi32.php)       | Windows 'gdi32' wrapper
| [std.xinput](std.xinput.php)     | Windows 'direct X input' wrapper
| [std.xaudio2](std.xaudio2.php)   | 'xaudio2' wrapper
