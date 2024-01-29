<?php

namespace HW\Factory;

use HW\Interfaces\IUserService;
use HW\Lib\UserService;

class UserServiceFactory extends AbstractFactory
{
    static protected function getDefault(): string
    {
        return UserService::class;
    }

    static public function get(...$args): IUserService {
        return parent::get(...$args);
    }
}