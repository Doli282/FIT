<?php declare(strict_types=1);

namespace HW\Lib;

use HW\Interfaces\IMathUtils;
use InvalidArgumentException;

class MathUtils implements IMathUtils
{
    /**
     * Sum a list of numbers.
     */
    public function sum(array $list): int
    {
        $sum = 0;

        foreach ($list as $item){
            if (!is_numeric($item) || !(is_finite((float)$item))) {
                throw new InvalidArgumentException();
            }
            $sum += $item;
        }

        return (int)($sum);
    }

    /**
     * Solve linear equation ax + b = 0.
     */
    public function solveLinear($a, $b): float|int
    {
        if (!(is_numeric($a) && is_numeric($b)) ||
            !(is_finite((float)$a) && is_finite((float)$b))) {
            throw new InvalidArgumentException();
        }
        if ($a == 0) {
            throw new InvalidArgumentException();
        }

        return ((-$b) / $a);
    }

    /**
     * Solve quadratic equation ax^2 + bx + c = 0.
     *
     * @return array Solution x1 and x2.
     */
    public function solveQuadratic($a, $b, $c): array
    {
        if (!(is_numeric($a) && is_numeric($b) && is_numeric($c)) ||
            !(is_finite((float)$a) && is_finite((float)$b) && is_finite((float)$c))) {
            throw new InvalidArgumentException();
        }
        if ($a == 0) {
            return array($this->solveLinear($b, $c));
        }

        $exp = pow($b, 2);
        $d = sqrt( $exp - (4 * $a * $c));
        if(!is_finite($d))
        {
            return [];
        }
        $x1 = (-$b + $d) / (2 * $a);
        $x2 = (-$b - $d) / (2 * $a);
        if($x1 == $x2)
        {
            return [$x1];
        }
        return [$x1, $x2];
    }
}
