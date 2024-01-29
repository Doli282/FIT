<?php

namespace HW\Factory;

abstract class AbstractFactory
{
    static private array $builder = [];

    abstract static protected function getDefault(): string;

    static public function setBuilder(string $class): void {
        self::$builder[static::class] = $class;
    }

    static public function clear(): void {
        unset(self::$builder[static::class]);
    }

    static public function get(...$args): mixed {
        if(!isset(self::$builder[static::class])) {
            self::setBuilder(static::getDefault());
        }
        return new self::$builder[static::class](...$args);
    }
}