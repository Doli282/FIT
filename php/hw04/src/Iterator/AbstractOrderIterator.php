<?php declare(strict_types=1);

namespace Iterator;

use Node;

abstract class AbstractOrderIterator implements \Iterator
{
    // TODO: shared attributes?
    /**
     * @var Node[]
     */
    protected array $stack;
    protected int $position = 0;
    public function __construct(Node $root)
    {
        // TODO: Implement constructor.
        $this->rewind();
        $this->createStack($root);
    }

    public function current(): ?Node
    {
        // TODO: Implement current() method.
        return $this->stack[$this->position];
    }

    public function next(): void
    {
        // TODO: Implement next() method.
        $this->position++;
    }

    public function key(): bool|int|float|string|null
    {
        // TODO: Implement key() method.
        return $this->position;
    }

    public function valid(): bool
    {
        // TODO: Implement valid() method.
        return isset($this->stack[$this->position]);
    }

    public function rewind(): void
    {
        // TODO: Implement rewind() method.
        $this->position = 0;
    }

    protected function createStack(Node $currentNode) : void{}
}
