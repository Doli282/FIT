<?php declare (strict_types=1);

class Bag
{
    public function add(mixed $item): void
    {
        // TODO
        $this->numberOfElements++;
        $this->collection[] = $item;
        return;
    }

    public function clear(): void
    {
        // TODO
        $this->numberOfElements = 0;
        unset($this->collection);
        $this->collection = array();
        return;
    }

    public function contains(mixed $item): bool
    {
        // TODO
        return in_array($item, $this->collection, true);
    }

    public function elementSize(mixed $item): int
    {
        // TODO
        $count = 0;
        foreach ($this->collection as $element)
        {
            if($element === $item)
            {
                $count++;
            }
        }
        return $count;
    }

    public function isEmpty(): bool
    {
        // TODO
        return ($this->numberOfElements == 0);
    }

    public function remove(mixed $item): void
    {
        // TODO
        $itemKey = array_search($item, $this->collection, true);
        if($itemKey === false)
        {
            return;
        }
        unset($this->collection[$itemKey]);
        $this->numberOfElements--;
        return;
    }

    public function size(): int
    {
        // TODO
        return $this->numberOfElements;
    }

    protected mixed $collection = []; // array with all elements
    protected int $numberOfElements = 0; // number of elements in the array - so I do not have to count it everytime the time....
}
