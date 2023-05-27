<header class="sticky top-0 z-50">
    <div class="lg:m-auto w-full lg:w-[75rem] flex justify-between items-center p-2">
        <a href="index.php"><img src="imgs/swag-logo.jpg" alt="SWAG" class="w-24 lg:w-50" /></a>
        <ul class="text-white flex text-md">
            <li class="mr-5"><a href="index.php" class="<?=$_SERVER['REQUEST_URI'] == '/index.php' ? 'border-b' : ''?> hover:border-b">Home</a></li>
            <li class="mr-5"><a href="language.php" class="<?=$_SERVER['REQUEST_URI'] == '/language.php' ? 'border-b' : ''?> hover:border-b">Overview</a></li>
            <li class="mr-5"><a href="std/swag.runtime.html" target="_blank" class="hover:border-b">Runtime</a></li>
            <li class="mr-5"><a href="libraries.php" class="<?=$_SERVER['REQUEST_URI'] == '/libraries.php' ? 'border-b' : ''?> hover:border-b">Std</a></li>
            <li><a href="https://github.com/swag-lang/swag" target="_blank" class="hover:border-b">Github</a></li>
        </ul>
    </div>
</header>