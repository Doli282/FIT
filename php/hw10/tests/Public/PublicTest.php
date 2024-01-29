<?php

namespace Public;

use Star\StarCommand;
use Symfony\Component\Console\Application;
use Symfony\Component\Console\Command\Command;
use Symfony\Component\Console\Input\ArgvInput;
use Symfony\Component\Console\Input\ArrayInput;
use Symfony\Component\Console\Output\BufferedOutput;

class PublicTest extends \PHPUnit\Framework\TestCase
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
/*
    public function testBasic() {
        $studentCommand = new StarCommand();
        $r = $this->generateImageUsingCommand($studentCommand, [
            "star",
            "480",
            "16727100",
            "6",
            "0.8",
            __DIR__ . "/student-star.png",
            "4342338",
            "10526880",
            "7"
        ]);
        $likeliness = $this->imageComparator(__DIR__ . "/example-star.png", __DIR__ . "/student-star.png");
        unlink(__DIR__ . "/student-star.png");
        $this->assertEmpty($r);
        $this->assertLessThan(0.25, $likeliness);
    }
*/}
