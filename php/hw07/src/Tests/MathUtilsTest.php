<?php declare(strict_types=1);

namespace HW\Tests;

use HW\Factory\MathUtilsFactory;
use PHPUnit\Framework\TestCase;

class MathUtilsTest extends TestCase
{
    private function getMathUtils() {
        return MathUtilsFactory::get();
    }

    // ------- test sum ----------
    /** @dataProvider _intArrayProvider */
    public function testSum($array) : void
    {
        $mathUtil = $this->getMathUtils();
        $this->assertEquals((int)(array_sum($array)), $mathUtil->sum($array));
    }

    public function testSumRound() : void
    {
        $mathUtil = $this->getMathUtils();
        $this->assertEquals(5, $mathUtil->sum(array(0.2, 5)));
        $this->assertEquals(0, $mathUtil->sum(array(-0.2, 0)));
        $this->assertEquals(-1, $mathUtil->sum(array(0.2, -1.4)));
        $this->assertEquals(6, $mathUtil->sum(array(6.999, 0)));
        $this->assertEquals(8, $mathUtil->sum(array(0.2, "8")));
        $this->assertEquals(7, $mathUtil->sum(array("0.2", 7)));
    }

    /** @dataProvider _NaNProvider3 */
    public function testFailingSum($a, $b, $c) : void
    {
        $array = array($a, $b, $c);
        $mathUtil = $this->getMathUtils();
        $this->expectException(\InvalidArgumentException::class);
        $mathUtil->sum($array);
    }

    // ------- test linear ----------
    /** @dataProvider _zeroProvider */
    public function testFailingSolveLinearDivisionByZero($a) : void
    {
        $mathUtil = $this->getMathUtils();
        $this->expectException(\InvalidArgumentException::class);
        $mathUtil->solveLinear($a, 1);
    }

    /** @dataProvider _NaNProvider2 */
    public function testFailingSolveLinearNotANumber($a, $b) : void
    {
        $mathUtil = $this->getMathUtils();
        $this->expectException(\InvalidArgumentException::class);
        $mathUtil->solveLinear($a, $b);
    }

    /** @dataProvider _linearProvider */
    public function testSolveLinear($a, $b, $x) : void
    {
        $mathUtil = $this->getMathUtils();
        $this->assertEqualsWithDelta($x, $mathUtil->solveLinear($a, $b), 0.0001);
    }

    // ------- test quadratic ----------
    /** @dataProvider _quadraticProviderTwoSolutions */
    public function testSolveQuadraticTwoSolutions($a, $b, $c, $x1, $x2) : void
    {
        $mathUtil = $this->getMathUtils();
        $res = $mathUtil->solveQuadratic($a, $b, $c);
        $this->assertEqualsCanonicalizing([$x1, $x2], $res);
    }

    /** @dataProvider _quadraticProviderOneSolution */
    public function testSolveQuadraticOneSolution($a, $b, $c, $x1) : void
    {
        $mathUtil = $this->getMathUtils();
        $this->assertEqualsWithDelta([$x1], $mathUtil->solveQuadratic($a, $b, $c), 0.01);
    }

    /** @dataProvider _invalidQuadraticProvider */
    public function testFailingSolveQuadraticImaginary($a, $b, $c) : void
    {
        $mathUtil = $this->getMathUtils();
        $res = $mathUtil->solveQuadratic($a, $b, $c);
        $this->assertEquals([], $res);
    }

    /** @dataProvider _NaNProvider3 */
    public function testFailingSolveQuadraticNaN($a, $b, $c) : void
    {
        $mathUtil = $this->getMathUtils();
        $this->expectException(\InvalidArgumentException::class);
        $mathUtil->solveQuadratic($a, $b, $c);
    }

    /** @dataProvider _zeroProvider
     *  @depends testSolveLinear
     */
    public function testSolveQuadraticFallbackToLinearZero($a) : void
    {
        $b = 1;
        $c = 3;
        $mathUtil = $this->getMathUtils();
        $resQuadratic = $mathUtil->solveQuadratic($a, $b, $c);
        $resLinear = $mathUtil->solveLinear($b, $c);
        $this->assertEqualsWithDelta([$resLinear], $resQuadratic, 0.00001);

    }


    /** @dataProvider _linearProvider
     *  @depends testSolveLinear
     */
    public function testSolveQuadraticFallbackToLinear($a, $b, $res) : void
    {
        $mathUtil = $this->getMathUtils();
        $resQuadratic = $mathUtil->solveQuadratic(0, $a, $b);
        //$resLinear = $mathUtil->solveLinear($a, $b);
        $this->assertEqualsWithDelta([$res], $resQuadratic, 0.00001);

    }


    /**
     * @throws \Exception
     */
    private function _intArrayProvider() : \Generator
    {
        $array = array();
        yield[$array];
        for($repetition = 0; $repetition < 5; $repetition++)
        {
            for($i = 0; $i  < 10; $i++){
                $array[random_int(-10, 10)] = random_int(-100, 100);
            }
            yield[$array];
        }
    }

    private function _zeroProvider() : array
    {
        return [
            [0], [0.0], ["0"], ['0'], ["    0   "]
        ];
    }

    private function _linearProvider(): array
    {
        return [
            [1, -1, 1],
            [1, 0, 0],
            [1, 1, -1],
            [2, -1, 0.5],
            [3, 1, -1/3],
            [6, 2, -0.333333],
            [3, 2, -0.66667],
            [0.5, 1/2, -1]
        ];
    }

    private function _quadraticProviderTwoSolutions() : array
    {
        return [
            [1, 2, 0, 0, -2],
            [1, 0, -4, 2, -2],
//            [10, 10, -10, 0.61803, -1.6180],
            [-1, 5, 10, (5/2)-(sqrt(65)/2), (5/2)+(sqrt(65)/2)],
            [-1, 5/2, 10, (5/4)-(sqrt(185)/4), (5/4)+(sqrt(185)/4)],
            [1, 2, 0, -2, 0],
            [1, 0, -4, -2, 2],
//            [10, 10, -10, -1.6180, 0.61803],
            [-1, 5, 10, (5/2)+(sqrt(65)/2), (5/2)-(sqrt(65)/2)],
            [-1, 5/2, 10, (5/4)+(sqrt(185)/4), (5/4)-(sqrt(185)/4)]
        ];
    }

    private function _quadraticProviderOneSolution() : array
    {
        return [
            [1, 2, 1, -1],
            [1, -4, 4, 2],
            [1, 0, 0, 0],
        ];
    }

    private function _invalidQuadraticProvider() : array
    {
        return [
            [10, -5, 10],
            [1, 2, 2],
            [1, 0, 4]
        ];
    }

    private function _NaNProvider2() : array
    {
        return [
            ['a', 1],
            [null, 1],
            [NAN, 1],
            [INF, 1],
            [1, 'a'],
            [1, null],
            [1, NAN],
            [1, INF]
        ];
    }
    private function _NaNProvider3() : array
    {
        return [
            ['a', 1, 1],
            [null, 1, 1],
            [NAN, 1, 1],
            [INF, 1, 1],
            [1, 'a', 1],
            [1, null, 1],
            [1, NAN, 1],
            [1, INF, 1],
            [1, 1, 'a'],
            [1, 1, null],
            [1, 1, NAN],
            [1, 1, INF]
        ];
    }
}
