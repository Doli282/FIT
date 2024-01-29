<?php declare(strict_types=1);

namespace HW\Interfaces;

use InvalidArgumentException;

/**
 * DO NOT IMPLEMENT, WE ALREADY HAVE THE IMPLEMENTATION
 * FOR TESTING, USE MOCK INSTEAD...
 */
interface IStorage
{
    /**
     * @param $key mixed id by which is value stored
     * @param $value mixed stored value
     * @return void none
     * @throws InvalidArgumentException when $key is not string
     */
    function save($key, $value);

    /**
     * @param $key mixed for search ov value
     * @return mixed when id is known
     * @return null when id is not known
     * @throws InvalidArgumentException when $key is not string
     */
    function get($key);
}
