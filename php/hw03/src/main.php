<?php declare (strict_types=1);

// TODO - Implement autoloader
$autoloader = function($class) : void{
    $path =  __DIR__ . DIRECTORY_SEPARATOR . "classes" . DIRECTORY_SEPARATOR . str_replace('\\',DIRECTORY_SEPARATOR, $class) . '.php';
    if(file_exists($path))
    {
        require_once $path;
    }
};

spl_autoload_register($autoloader);