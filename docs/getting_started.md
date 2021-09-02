# You first install

## Windows 10

##### Swag needs to know the Sdk

In order to transform your code in a Windows executable, you will have to install the `Windows Sdk 10.0.19041.0`.

https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk/

It should probably work with a more recent version of the sdk, but this is not guaranteed (sometimes some functions are moved around by Microsoft between the Universal runtime and the Msvc runtime).

##### Swag needs to know its home

The first thing to do is to open a console window, go to the directory where the executable `swag.exe` is located, and call `swag env`.

This will set an environment variable called `SWAG_FOLDER` with the path to the executable, and this will change the system `PATH` variable for the current user.

```
$ cd swag
$ swag env
'SWAG_FOLDER' has been changed to 'F:/swag'
'PATH' environment variable has been changed
```

Close and relaunch the console, and you should be able to call *swag.exe* from everywhere.

> If Swag cannot be found, consider restarting your windows session.

This must be done only once, or each time you decide to change the location of the compiler.

##### Windows Defender realtime protection
It's activated by default under Windows 10, and runs each time you launch an executable or a process.
This can take some time, and increase the compile time of Swag **a lot**.

Consider excluding your Swag folder from it !

https://support.microsoft.com/en-us/windows/add-an-exclusion-to-windows-security-811816c0-4dfd-af4a-47e4-c301afe13b26#:~:text=Go%20to%20Start%20%3E%20Settings%20%3E%20Update,%2C%20file%20types%2C%20or%20process.

This is due to the fact that, mostly with the **test** and **run** commands, Swag will launch executables by its own, which will trigger Windows Defender on those new processes (i guess...)

# Your first project

The compile unit of swag is a **workspace** which contains a variable number of **modules**.
A module will compile to a dynamic library or an executable.

To create a fresh new workspace named "first" :

```
$ swag new -w:first'
=> workspace 'F:/first' has been created
=> module 'first' has been created
=> type 'swag run -w:F:\first' to build and run that module
```

This will also create a simple executable module *first* to print "Hello world !".

*F:/first/modules/first/src/main.swg*

``` csharp
#main
{
    @print("Hello world!\n")
}
```

A workspace contains a predefined number of sub folders :
* `modules/` contains all the modules (sub folders) of that workspace.
* `output/` (generated) contains the result of the build (this is where the executable will be located).
* `tests/` (optional) contains a list of test modules.
* `examples/` (optional) contains a list of 'examples' modules.
* `dependencies/` (generated) contains a list of external modules needed to compile the workspace.

A module is also organized in a predefined way :
* `moduleName/` the folder name of the module is used to create output files.
* `src/` contains the source code.
* `public/` (generated) will contain all the exports needed by other modules to use that one (in case of a dynamic library).
* `publish/` contains additional files to use that module (like an external C dll).

A module also always contains a special file named `module.swg`. This file is used to configure the module, and is **mandatory**.

#### To compile your workspace
```
$ swag build -w:first
            Workspace first [fast-debug-windows-x86_64]
             Building first
                 Done 0.067s
```

You can omit the workspace name (`-w:first` or `--workspace:first`) if you call swag directly from the workspace folder.
This command will compile all modules in `modules/` and `examples/`.

You can also build and run your workspace.

```
$ swag run -w:first
            Workspace first [fast-debug-windows-x86_64]
             Building first
      Running backend first
Hello world!
                 Done 0.093s
```

# Content of the Swag folder
The Swag folder contains the compiler `swag.exe`, but also a bunch of sub folders.
* `examples/` is a workspace with various code examples (mostly for testing).
* `reference/` is a workspace which contains an overview of the language, in the form of small tests.
* `testsuite/` is a workspace which contains all the tests to debug the compiler.
* `runtime/` contains the compiler runtime, which is included in all user modules.
* `std/` is the *standard* workspace which contains the *standard* modules that come with the compiler. A big work in progress.

# The Swag language
You should take a look at the `reference/` sub folder in the Swag directory. It contains the list of all that can be done with the language, in the form of small tests (in fact it's not really exhaustive, but should be...).

It's a good starting point to familiarize yourself with the language.

And as this is a normal Swag workspace, you could also build and test it with `swag test -w:swag/reference`.