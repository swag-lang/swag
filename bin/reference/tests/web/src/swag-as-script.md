# Script file

Swag can be used to build and run a simple script file, thanks to the fact that the compiler can run anything at compile time.
No executable will be generated, the compiler will do all the job.
To create a new script file with the special extension 'swgs':

    $ swag new -f:myScript
    => script file 'myScript.swgs' has been created
    => type 'swag script -f:myScript.swgs' to run that script

This will generate a simple file with a '#dependency' block and the program entry point '#main'.

```swag
// Swag script file
#dependencies
{
    // Here you can add your external dependencies
    // #import "core" location="swag@std"
}

#main
{
    @print("Hello world !\n")
}
```

You can then run your script with the 'script' command.

    $ swag script -f:myScript
    Hello world !

You can also just specify the script file **with the extension** as a command.

    $ swag myScript.swgs
    Hello world !

As examples, you will find a bunch of small scripts in `swag/bin/examples/scripts`.
To run one of them from the console, go to the folder and type for example `swag flappy.swgs`.

<html>
<div align="center">
    <div class="round-button">
        <a href="flappy.php" class="no-decoration">Flappy Bird</a>
    </div>
</div>
</html>

## Dependencies

You can add external dependencies, and they will be compiled and used as native code.

```swag
#dependencies
{
    // Import the standard module `core` from the swag standard workspace (which comes with the compiler)
    #import "core" location="swag@std"
}
```

A special hidden workspace (in the Swag cache folder) will be created to contain all the corresponding native code.

* To locate the Swag cache folder, add `--verbose-path` to the command line.
* To force the build of dependencies, add `--rebuildall` to the command line.

## More than one script file

If your script is divided in more than one single file, you can add `#load <filename>` in the `#dependencies` block.

```swag
#dependencies
{
    #load "myOtherFile.swgs"
    #load "folder/myOtherOtherFile.swgs"
}
```

## Debug

The compiler comes with a **bytecode debugger** that can be used to trace and debug compile time execution.
Add `@breakpoint()` in your code when you want the debugger to trigger.

The debugger command set is inspired by [Pdb](https://docs.python.org/3/library/pdb.html), the python debugger.
