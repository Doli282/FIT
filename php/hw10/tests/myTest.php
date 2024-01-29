<?php

use Star\StarCommand;
use Symfony\Component\Console\Application;
use Symfony\Component\Console\Command\Command;
use Symfony\Component\Console\Input\ArgvInput;
use Symfony\Component\Console\Input\ArrayInput;
use Symfony\Component\Console\Output\BufferedOutput;

class myTest extends \PHPUnit\Framework\TestCase
{

    protected function imageComparator(string $ref, string $student): float {
        $ref = imagecreatefrompng($ref);
        $student = imagecreatefrompng($student);
        $this->assertEquals(imagesx($ref), imagesx($student), "Width is not the same");
        $this->assertEquals(imagesy($ref), imagesy($student), "Height is not the same");
        $resX = imagesx($ref);
        $resY = imagesy($ref);
        $sum=0;
        for($x=0;$x<$resX;++$x){
            for($y=0;$y<$resY;++$y){
                $bytesRef=imagecolorat($ref,$x,$y);
                $bytesStudent=imagecolorat($student,$x,$y);
                $colorsRef=imagecolorsforindex($ref,$bytesRef);
                $colorsStudent=imagecolorsforindex($student,$bytesStudent);
                $value1=round(sqrt(0.2126*$colorsRef['red']**2+0.7152*$colorsRef['green']**2+ 0.0722*$colorsRef['blue']**2));
                $value2=round(sqrt(0.2126*$colorsStudent['red']**2+0.7152*$colorsStudent['green']**2+ 0.0722*$colorsStudent['blue']**2));
                $res = abs($value1-$value2)**2 / (255*255);
                $sum+=$res;
            }
        }
        return $sum/($resX*$resY);
    }

    protected function generateImageUsingCommand(Command $command, array $args): string {
        $buffer = new BufferedOutput();
        $application = new Application();
        $application->setAutoExit(false);
        $application->add($command);
        $application->setDefaultCommand($command->getName(), true);
        $application->run(new ArgvInput($args), $buffer);
        return $buffer->fetch();
    }

    /** @dataProvider _testCases */
    public function testReference($reference, $width, $color, $points, $radius, $bgColor = null, $borderColor = null, $borderWidth = null) {
        $studentCommand = new StarCommand();
        $r = $this->generateImageUsingCommand($studentCommand, [
            "star",
            $width,
            $color,
            $points,
            $radius,
            __DIR__ . "/student-star.png",
            $bgColor,
            $borderColor,
            $borderWidth
        ]);
        $likeliness = $this->imageComparator(__DIR__.DIRECTORY_SEPARATOR.'..'.DIRECTORY_SEPARATOR.'reference'.DIRECTORY_SEPARATOR.$reference,
            __DIR__ . "/student-star.png");
        unlink(__DIR__ . "/student-star.png");
        echo "[{$reference}]: likeness: {$likeliness}".PHP_EOL;
        $this->assertEmpty($r);
        $this->assertLessThan(0.25, $likeliness);
    }

    public static function _testCases() : array
    {
        return [
//            ['star 200 100 2 0.6 file.png 404040.png', 200, 100, 2, 0.6, 404040],
            ['star 40 808080 12 0.2.png', 40, 808080, 12, 0.2],
//            ['star 400 450120 5 0.4 file.png 888888 5 20.png', 400, 450120, 5, 0.4, 888888, 5, 20],
//            ['star 400 450120 5 0.4 file.png 888888 5 5.png', 400, 450120, 5, 0.4, 888888, 5, 5],
//            ['star 400 450120 5 0.9 file.png 888888 5 100.png', 400, 450120, 5, 0.9, 888888, 5, 100],
//            ['star 400 450120 5 0.9 file.png 888888 5 5.png', 400, 450120, 5, 0.9, 888888, 5, 5]
        ];
    }
}

/*
 * 0.015410861207228 ... norm
 * 0.018665311418685 ... bez zavorek
 * 0.015410861207228 ... sin zavorka
 */
