<?php
include('html_start.php');
include('header.php');
?>

<div class="lg:m-auto lg:w-[76rem] pt-2 lg:pt-10 overflow-hidden p-5">

    <div class="text-2xl">Standard Modules</div>
    <ul class="mt-5 ml-5 list-disc">

        <?php
        $files = [];
        $dir = 'std/';
        $handle = opendir($dir);

        while ($file = readdir($handle))
            if (is_file($dir . $file) && $file != 'swag.runtime.html')
                $files[] = $file;

        sort($files);

        foreach ($files as $file) {
            ?>

            <li>
                <a href="<?=$dir . $file?>" class="hover:underline" target="_blank"><?=str_replace('.html', '', $file)?></a>
            </li>

            <?php
        }
        ?>

    </ul>
</div>

<?php include('html_end.php'); ?>