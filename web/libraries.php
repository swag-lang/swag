<?php
include('html_start.php');
include('header.php');
?>

<div class="lg:m-auto lg:w-[70rem] pt-2 lg:pt-10 overflow-hidden p-5">
   
    <div class="text-2xl">Standard Modules</div>


        <?php
        $files = [];
        $dir = 'std/';
        $handle = opendir($dir);
        while($file = readdir($handle)) {
            if (is_file($dir . $file)) {
                $files[] = $file;
            }
        }
        sort($files);
        foreach ($files as $file) {
            if($file != "swag.runtime.html") {
            ?>
                <li>
                    <a href="<?=$dir . $file?>" class="hover:underline" target="_blank"><?=str_replace('.html', '', $file)?></a>
                </li>
            <?php
            }
        }
        ?>
    
</div>

<?php include('html_end.php'); ?>