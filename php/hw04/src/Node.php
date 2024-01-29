<?php declare(strict_types=1);

class Node extends BaseNode implements IteratorAggregate
{
    // Implement me
    public function getIterator() : Traversable
    {
        return new \Iterator\InOrderIterator($this);
    }
}
