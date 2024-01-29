<?php declare(strict_types=1);

// TODO implement and register autoloader
$autoloader = function($class) : void{
    $path =  __DIR__ . DIRECTORY_SEPARATOR . "src" . DIRECTORY_SEPARATOR . str_replace('\\',DIRECTORY_SEPARATOR, $class) . '.php';
    if(file_exists($path))
    {
        require_once $path;
    }
};

spl_autoload_register($autoloader);
