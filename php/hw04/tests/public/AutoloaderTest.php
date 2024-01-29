<?php declare(strict_types=1);

use PHPUnit\Framework\TestCase;

class AutoloaderTest extends TestCase
{
    protected const RESULT_CHAR_FAILED = "\033[31m × \033[39m";
    protected const RESULT_CHAR_SUCCESS = "\033[32m ✓ \033[39m";
    public function testAutoloader(): void
    {
        $nodeClass = '\\Node';
        $iteratorClass = 'Iterator\\InOrderIterator';
        $iteratorPreOrderClass = 'Iterator\\PreOrderIterator';
        $iteratorPostOrderClass = 'Iterator\\PostOrderIterator';
        $this->assertTrue(class_exists($nodeClass), self::RESULT_CHAR_FAILED . 'Can not autoload ' . $nodeClass . ' class!');
        $this->assertTrue(class_exists($iteratorClass), self::RESULT_CHAR_FAILED . 'Can not autoload ' . $iteratorClass . ' class!');
        $this->assertTrue(class_exists($iteratorPreOrderClass), self::RESULT_CHAR_FAILED . 'Can not autoload ' . $iteratorPreOrderClass . ' class!');
        $this->assertTrue(class_exists($iteratorPostOrderClass), self::RESULT_CHAR_FAILED . 'Can not autoload ' . $iteratorPostOrderClass . ' class!');
        print self::RESULT_CHAR_SUCCESS . "Autoloader test successfully passed!\n\n";
    }
}
