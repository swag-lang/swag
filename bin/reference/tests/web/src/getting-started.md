# Your first install
[Download](https://github.com/swag-lang/swag/releases) the latest release from github, and unzip it in a folder. Of course a *SSD* is better.

### Note for Windows 10/11

#### Windows SDK


You must install the latest version of the **Windows 10 SFK** in order to be able to build an executable for Windows. 
Otherwhise the compiler will complain, and exit.
You can try [here](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/)

#### PATH

You should register the location of the swag compiler (`swag.exe`) in the 'PATH' environment variable to be able to call it from everywhere.

You can open a *Powershell* window, and run the following code :

    # You must replace `c:\swag-lang\swag\bin` with your location of `swag.exe`
    [Environment]::SetEnvironmentVariable(
        "Path",
        [Environment]::GetEnvironmentVariable("Path", "User") + ";c:\swag-lang\swag\bin",
        "User"
    )

#### Windows Defender

The Windows Defender realtime protection is activated by default, and runs each time you launch an executable.
This can increase the compile time of your project, so consider excluding your Swag folder from it !

[Reference](https://support.microsoft.com/en-us/windows/add-an-exclusion-to-windows-security-811816c0-4dfd-af4a-47e4-c301afe13b26#:~:text=Go%20to%20Start%20%3E%20Settings%20%3E%20Update,%2C%20file%20types%2C%20or%20process)

Under Windows 11, there's also something called the *Smart App Control*, which can also have a great impact on execution time if it is activated (or in evaluation mode).

[Reference](https://support.microsoft.com/en-us/topic/what-is-smart-app-control-285ea03d-fa88-4d56-882e-6698afdb7003)

# Your first project

The compile unit of swag is a **workspace** which contains a variable number of **modules**.
A module will compile to a dynamic library or an executable.

To create a fresh new workspace named *first* :

    $ swag new -w:first
    => workspace 'F:/first' has been created
    => module 'first' has been created
    => type 'swag run -w:F:\first' to build and run that module

This will also create a simple executable module *first* to print "Hello world !".

*F:/first/modules/first/src/main.swg*

```swag
#main
{
    @print("Hello world!\n")
}
```

A workspace contains a predefined number of sub folders:

* `modules/` contains all the modules (sub folders) of that workspace.
* `output/` (generated) contains the result of the build (this is where the executable will be located).
* `tests/` (optional) contains a list of test modules.
* `dependencies/` (generated) contains a list of external modules needed to compile the workspace.

A module is also organized in a predefined way:

* `moduleName/` the folder name of the module is used to create output files.
* `src/` contains the source code.
* `public/` (generated) will contain all the exports needed by other modules to use that one (in case of a dynamic library).
* `publish/` contains additional files to use that module (like an external C dll).

A module always contains a special file named `module.swg`. This file is used to configure the module, and is **mandatory**.

### To compile your workspace

    $ swag build -w:first
                Workspace first [fast-debug-windows-x86_64]
                Building first
                    Done 0.067s

You can omit the workspace name (`-w:first` or `--workspace:first`) if you call the compiler directly from the workspace folder.
This command will compile all modules in `modules/`.

You can also build and run your workspace.

    $ swag run -w:first
                Workspace first [fast-debug-windows-x86_64]
                Building first
        Running backend first
    Hello world!
                Done 0.093s

# Content of the Swag folder
The Swag folder contains the compiler `swag.exe`, but also a bunch of sub folders.

* `reference/` is a workspace which contains an overview of the language, in the form of small tests.
* `testsuite/` is a workspace which contains all the tests to debug the compiler.
* `runtime/` contains the compiler runtime, which is included in all user modules.
* `std/` is the [standard workspace](std.php) which contains all the standard modules that come with the compiler. A big work in progress.

# The Swag language
You should take a look at the `reference/` sub folder in the Swag directory, or to the corresponding generated [documentation](language.php).
It contains the list of all that can be done with the language, in the form of small tests (in fact it's not really exhaustive, but should be...).

It's a good starting point to familiarize yourself with the language.

And as this is a normal Swag workspace, you could also build and test it with `swag test -w:swag/reference`.

You will also find some small examples (mostly written for tests) in `swag/bin/examples/modules`.
To build and run one of them from the console, go to the workspace folder (`/examples`) and type for example `swag run -m:wnd`.
