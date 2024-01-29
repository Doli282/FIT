<?php declare(strict_types=1);

use Iterator\InOrderIterator;
use Iterator\PostOrderIterator;
use Iterator\PreOrderIterator;
use Tester\TreeTestHelper;
use PHPUnit\Framework\TestCase;

class IteratorTest extends TestCase
{
    protected const RESULT_CHAR_FAILED = "\033[31m × \033[39m";
    protected const RESULT_CHAR_SUCCESS = "\033[32m ✓ \033[39m";

    /** @var TreeTestHelper[] */
    protected array $trees = [];

    public function setUp(): void
    {
        global $trees;
        $this->trees = $trees;
        set_time_limit(30);
    }

    public function testInOrderIterator(): void
    {
        foreach ($this->trees as $tree) {
            $iter = new InOrderIterator($tree->getRoot());
            $res = $tree->treeToString($iter);
            $this->assertEquals($tree->getIn(), $res, self::RESULT_CHAR_FAILED . 'In Order: Incorrect output for tree with id: '. $tree->getId());
        }
        print self::RESULT_CHAR_SUCCESS . "Tests for in order iterator successfully passed!\n\n";
    }

    public function testPreOrderIterator(): void
    {
        foreach ($this->trees as $tree) {
            $iter = new PreOrderIterator($tree->getRoot());
            $res = $tree->treeToString($iter);
            $this->assertEquals($tree->getPre(), $res, self::RESULT_CHAR_FAILED . 'Pre Order: Incorrect output for tree with id: '. $tree->getId());
        }
        print self::RESULT_CHAR_SUCCESS . "Tests for pre order iterator successfully passed!\n\n";
    }

    public function testPostOrderIterator(): void
    {
        foreach ($this->trees as $tree) {
            $iter = new PostOrderIterator($tree->getRoot());
            $res = $tree->treeToString($iter);
            $this->assertEquals($tree->getPost(), $res, self::RESULT_CHAR_FAILED . 'Post Order: Incorrect output for tree with id: '. $tree->getId());
        }
        print self::RESULT_CHAR_SUCCESS . "Tests for post order iterator successfully passed!\n\n";
    }

    public function testIteratorAggregate(): void
    {
        try {
            foreach ($this->trees as $tree) {
                $iter = $tree->getRoot()->getIterator();
                $res = $tree->treeToString($iter);
                $this->assertEquals($tree->getIn(), $res, self::RESULT_CHAR_FAILED . 'Aggregate: Incorrect output for tree with id: '. $tree->getId());
            }
        } catch (Exception $e) {
            $this->fail(self::RESULT_CHAR_FAILED . 'Tests for iterator aggregate thrown an exception: '. $e->getMessage());
        }

        print self::RESULT_CHAR_SUCCESS . "Tests for iterator aggregate successfully passed!\n\n";
    }
}
