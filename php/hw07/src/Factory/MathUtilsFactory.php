<?php

namespace HW\Factory;

use HW\Interfaces\IMathUtils;
use HW\Lib\MathUtils;

class MathUtilsFactory extends AbstractFactory
{
    static protected function getDefault(): string
    {
        return MathUtils::class;
    }

    static public function get(...$args): IMathUtils {
        return parent::get(...$args);
    }
}