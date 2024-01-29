<?php

class Set extends Bag
{
    public function add(mixed $item): void
    {
        // TODO

        if($this->contains($item))
        {
            return;
        }

        $this->numberOfElements++;
        $this->collection[] = $item;
        return;
    }
    public function elementSize(mixed $item): int
    {
        // TODO
        if($this->contains($item))
        {
            return 1;
        }
        else{
            return 0;
        }
    }
}