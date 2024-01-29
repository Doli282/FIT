<?php declare(strict_types=1);

namespace Star;

use Symfony\Component\Console\Attribute\AsCommand;
use Symfony\Component\Console\Command\Command;
use Symfony\Component\Console\Input\InputArgument;
use Symfony\Component\Console\Input\InputInterface;
use Symfony\Component\Console\Output\OutputInterface;

#[AsCommand(name: 'star')]
class StarCommand extends Command
{
    protected function configure(): void
    {
        // set description
        $this->setName('star')
            ->setDescription('Create star image')
            ->setHelp('Create and save an image of a star/polygon');

        // set required arguments
        $this->addArgument('width', InputArgument::REQUIRED, 'width and height of the output image')
            ->addArgument('color', InputArgument::REQUIRED, 'color of the star')
            ->addArgument('points', InputArgument::REQUIRED, 'number of vertices of the star')
            ->addArgument('radius', InputArgument::REQUIRED, 'defines "fullness" of the polygon (value between 0.0 and 1.0)')
            ->addArgument('output', InputArgument::REQUIRED, 'name of the output file');

        // set optional arguments
        $this->addArgument('bgColor', InputArgument::OPTIONAL, 'background color (default=white)')
            ->addArgument('borderColor', InputArgument::OPTIONAL, 'border color (default=none)')
            ->addArgument('borderWidth', InputArgument::OPTIONAL, 'width of the order in px (default=none)');
    }

    protected function execute(InputInterface $input, OutputInterface $output): int
    {
        // load arguments
        $width = filter_var($input->getArgument('width'), FILTER_VALIDATE_INT);
        $colorArg = filter_var($input->getArgument('color'), FILTER_VALIDATE_INT);
        $points = filter_var($input->getArgument('points'), FILTER_VALIDATE_INT);
        $radius = filter_var($input->getArgument('radius'),FILTER_VALIDATE_FLOAT);
        $outputFile = $input->getArgument('output');

        $bgColorArg = $input->getArgument('bgColor');
        $borderColorArg = $input->getArgument('borderColor');
        $borderWidthArg = $input->getArgument('borderWidth');


        // create background image
        if($width === false || $width < 1)
        {
            $output->writeln('Width has to be a positive integer');
            return Command::FAILURE;
        }
        $midpoint = $width/2;
        $image = imagecreatetruecolor($width, $width);
        if($image === false)
        {
            $output->writeln('Background image could not be created');
            return Command::FAILURE;
        }
        if(imageantialias($image, true) === false)
        {
            $output->writeln('Antialiasing failed');
            return Command::FAILURE;
        }

        // set background color
        if(is_null($bgColorArg))
        {
            $bgColorRGB = ['red' => 255, 'blue' => 255, 'green' => 255];
        }
        else
        {
            $bgColorArg = filter_var($bgColorArg, FILTER_VALIDATE_INT);
            if($bgColorArg === false || $bgColorArg < 0 || $bgColorArg > 16777215) {
                $output->writeln('Background color must be integer between 0 and 16777215 or not set');
                return Command::FAILURE;
            }
            $bgColorRGB = $this->separateRGB($bgColorArg);
        }
        $bgColor = imagecolorallocate($image, $bgColorRGB['red'], $bgColorRGB['green'], $bgColorRGB['blue']);
        if($bgColor === false) {
            $output->writeln('Background color could not be allocated');
            return Command::FAILURE;
        }
        if(imagefill($image, 0, 0, $bgColor) === false) {
            $output->writeln('Background color could not be set');
            return Command::FAILURE;
        }
        imagecolordeallocate($image, $bgColor);

        // check radius
        if($radius === false || $radius < 0 || $radius > 1)
        {
            $output->writeln('Radius must be a number between 0 and 1');
            return Command::FAILURE;
        }
        // check points
        if($points === false || $points < 2)
        {
            $output->writeln('Points must be a positive integer greater than 1');
            return Command::FAILURE;
        }

        // DRAW BORDER if it is set
        if(!is_null($borderColorArg) && !is_null($borderWidthArg))
        {
            $borderColorArg = filter_var($borderColorArg, FILTER_VALIDATE_INT);
            $borderWidthArg = filter_var($borderWidthArg, FILTER_VALIDATE_INT);
            if($borderWidthArg === false || $borderWidthArg > $midpoint || $borderWidthArg < 1)
            {
                $output->writeln('Border width must be positive integer lower than or equal to "width"/2');
                return Command::FAILURE;
            }
            if($this->drawStar($image, $borderColorArg, $output, $midpoint, $points, $midpoint*$radius, $midpoint) === false)
            {
                $output->writeln('Problem occurred while drawing the border.');
                return Command::FAILURE;
            }
        }
        elseif(!is_null($borderColorArg) && is_null($borderWidthArg))
        {
            $output->writeln('borderColor must be set only together with borderWidth');
            return Command::FAILURE;
        }
        else
        {
            $borderWidthArg = 0;
        }

        // DRAW STAR:
        $outerRadius = $midpoint - $borderWidthArg;
        if(!$this->drawStar($image, $colorArg, $output, $midpoint, $points, $outerRadius*$radius, $outerRadius))
        {
            $output->writeln('Problem occurred while drawing the star.');
            return Command::FAILURE;
        }

        // save image
        $outputFile = $this->setOutputFile($outputFile);
        if(imagepng($image, $outputFile) === false)
        {
            $output->writeln("Image could not be save as '{$outputFile}'");
            return Command::FAILURE;
        }

        // clean
        imagedestroy($image);

        return 0;
    }

    // Convert RGB from one int value to three separate parts
    protected function separateRGB(int $rgbInt) : array
    {
        $MASK = 0xFF;

        if($rgbInt < 0){
            return ['red' => 0, 'blue' => 0, 'green' => 0];
        }
        if($rgbInt > 16777215)
        {
            return ['red' => 255, 'blue' => 255, 'green' => 255];
        }
        return [
            'blue' => $rgbInt & $MASK,
            'green' => ($rgbInt >> 8) & $MASK,
            'red' => ($rgbInt >> 16) & $MASK
        ];
    }

    /**
     * Calculate vertices of the star including the inner vertices
     *
     * @param float $midpoint center of the star
     * @param int $numberOfVertices number of all vertices including both inner and outer vertices
     * @param float $inRadius radius of the inner circle
     * @param float $circumRadius radius of the outer circle
     * @return array coordinates of the vertices [V1x, V1y, V2x, V2y, ...]
     */
    protected function calculateVertices(float $midpoint, int $numberOfVertices, float $inRadius, float $circumRadius) : array
    {
        $vertices = [];
        echo 'center:'.$midpoint.PHP_EOL;
        echo 'OUTER:'.$circumRadius.PHP_EOL;
        echo 'INNER:'.$inRadius.PHP_EOL;

        for ($i = 0; $i < $numberOfVertices; $i++)
        {
            if($i % 2 == 0)
            { // outer vertices
                // x coordinate

                $x = $midpoint - $circumRadius * sin(2*pi()*($i/$numberOfVertices));
                echo "midpoint=$midpoint|circum=$circumRadius|i=$i|vertices=$numberOfVertices".PHP_EOL;
                $vertices[] = $x;
                 // y coordinate
                $y = $midpoint - $circumRadius * cos(2*pi()*$i/$numberOfVertices);
                $vertices[] = $y;
            }
            else
            { // inner vertices
                // x coordinate
                $vertices[] = $x = $midpoint - $inRadius * sin(2*pi()*($i/$numberOfVertices));
                // y coordinate
                $vertices[] = $y = $midpoint - $inRadius * cos(2*pi()*($i/$numberOfVertices));
            }
            echo $i.': ['.$x.','.$y.']'.PHP_EOL;
        }

        return $vertices;
    }

    // Set name of the output file. Make sure it has .png extension.
    protected function setOutputFile(string $outputFileName) : string
    {
        if(!preg_match('/.*\.png$/', $outputFileName))
        {
            $outputFileName .='.png';
        }
        return $outputFileName;
    }

    protected function drawStar(\GdImage $image, int|bool $intRGB, OutputInterface $output, float|int $center, int $points, float $inradius, float $circumradius) : bool
    {
        // prepare color
        if($intRGB === false || $intRGB < 0 || $intRGB > 16777215)
        {
            $output->writeln('Color has to be int between 0 and 16777215');
            return false;
        }
        $colorRGB = $this->separateRGB($intRGB);
        $color = imagecolorallocate($image, $colorRGB['red'], $colorRGB['green'], $colorRGB['blue']);
        if($color === false) {
            $output->writeln('Color of the star could not be allocated');
            return false;
        }

        // prepare vertices and draw the star
        $vertices = $this->calculateVertices($center, $points*2, $inradius, $circumradius);
        if(imagefilledpolygon($image, $vertices, $color) === false)
        {
            $output->writeln('Unable to draw the star');
            return false;
        }

        imagecolordeallocate($image, $color);
        return true;
    }
}
