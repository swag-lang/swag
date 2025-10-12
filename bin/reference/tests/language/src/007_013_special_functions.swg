#global skip

/**
# Main Function (`#main`)
The '#main' function serves as the program's primary entry point — similar to C/C++'s 'main()'.
It is unique per module and marks where execution begins when the program starts.
*/
#main
{
}

/**
# Handling Program Arguments
Command-line arguments are accessed via the intrinsic '@args', which returns a slice
of strings representing all arguments passed to the program.
*/
#main
{
    var myArgs = @args
    var count = @countof(myArgs)

    if myArgs[0] == "fullscreen"
    {
        // Example: Enable fullscreen mode here
        ...
    }
}

/**
# Pre-Main Function (`#premain`)
The '#premain' function executes after all '#init' functions in every module have run,
but before '#main'.
It's ideal for setup tasks that depend on completed module initialization.
*/
#premain
{
}

/**
# Initialization Function (`#init`)
'#init' runs during module initialization.
Multiple '#init' functions can exist within a module and execute before '#premain' and '#main'.
Execution order between multiple '#init' functions is undefined.
*/
#init
{
}

/**
# Drop Function (`#drop`)
'#drop' acts as the cleanup counterpart to '#init'.
It executes when a module is unloaded, releasing any resources acquired during initialization.
Multiple '#drop' functions can exist, and they execute in reverse order relative to '#init'.
*/
#drop
{
}

/**
# Test Function (`#test`)
'#test' is used exclusively for testing code.
Functions marked with '#test' are executed only in test mode, allowing developers to
validate functionality through assertions and controlled test cases.
*/
#test
{
}
