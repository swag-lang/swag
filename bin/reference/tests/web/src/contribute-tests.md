# Contribute to the test suite

You can help by writing small tests to debug the compiler (and the compiler alone).

### Create a workspace and a test module

First create a workspace with the `--test` option.

```
$ swag new -w:myWorkspace --test
```

This will also create a test module `myWorkspace` located in `myWorkspace\tests`.

You can then compile and run the tests with the `test` command.

```
$ swag test -w:myWorkspace
```

To force the build, add `--rebuild`.

### Write tests

* Tests should be small, located in different **small files**.
* Just write files like `test1.swg`, `test2.swg` and so on, in the `\src` sub folder of your module.
* Here you are only testing the compiler, and not the standard libraries. So do not add dependencies to external modules.
* You can use `@print` temporary to see a result, but in the end tests should be silent. `@assert` is the way to go.
* `swag\bin\testsuite` is the official test suite for Swag. If your test module is cool, then it could ultimately find a place there.
* Note that if a test works as expected, **keep it**, and **write another one** ! This is useful for testing regression.
