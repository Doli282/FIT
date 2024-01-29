<?php

namespace HW\Interfaces;

use InvalidArgumentException;

interface IMathUtils
{
    /**
     * @param array $list array of numbers
     * @return int sum of all numbers in list, if not whole, only whole part is returned, empty = 0
     * @throws InvalidArgumentException when we are unable to calculate the sum cause of any error
     */
    public function sum(array $list): int;


    /**
     * Solves linear equation ax + b = 0.
     * @param $a mixed a value from equation
     * @param $b mixed b value from equation
     * @return float|int
     * @throws InvalidArgumentException when $a or $b value is not convertable to number or any other problems
     */
    public function solveLinear($a, $b): float|int;

    /**
     * Solve quadratic equation ax^2 + bx + c = 0.
     * @param $a mixed a value from equation
     * @param $b mixed b value from equation
     * @param $c mixed c value from equation
     * @return array list af all possible real unique solutions of X value.
     * @throws InvalidArgumentException when $a, $b or $c value is not convertable to a number
     */
    public function solveQuadratic($a, $b, $c): array;
}
