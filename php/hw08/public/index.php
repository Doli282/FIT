<?php declare(strict_types=1);

use Books\Rest\RestApp;

require __DIR__ . '/../vendor/autoload.php';

$controller = new RestApp();
$controller->configure();
$controller->run();
