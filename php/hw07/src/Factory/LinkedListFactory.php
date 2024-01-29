<?php

namespace HW\Factory;

use HW\Interfaces\ILinkedList;
use HW\Lib\LinkedList;

class LinkedListFactory extends AbstractFactory
{
    static protected function getDefault(): string
    {
        return LinkedList::class;
    }

    static public function get(...$args): ILinkedList {
        return parent::get(...$args);
    }
}