<?php declare(strict_types=1);

namespace HW\Lib;

use HW\Interfaces\ILinkedList;
use HW\Interfaces\ILinkedListItem;
use function PHPUnit\Framework\isNull;

class LinkedList implements ILinkedList
{
    protected ?ILinkedListItem $first = null;

    protected ?ILinkedListItem $last = null;

    public function getFirst(): ?ILinkedListItem
    {
        return $this->first;
    }

    public function setFirst(?ILinkedListItem $first): LinkedList
    {
        $this->first = $first;
        return $this;
    }

    public function getLast(): ?ILinkedListItem
    {
        return $this->last;
    }

    public function setLast(?ILinkedListItem $last): LinkedList
    {
        $this->last = $last;
        return $this;
    }

    /**
     * Place new item at the begining of the list
     */
    public function prependList(string $value): ILinkedListItem
    {
        $item = new LinkedListItem($value);
        $second = $this->getFirst();
        $this->setFirst($item);
        if(is_null($second))
        {
            $this->setLast($item);
        }
        else
        {
            $second->setPrev($item);
            $item->setNext($second);
        }

        return $item;
    }

    /**
     * Place new item at the end of the list
     */
    public function appendList(string $value): ILinkedListItem
    {
        $item = new LinkedListItem($value);
        $penultimate = $this->getLast();
        $this->setLast($item);
        if(is_null($penultimate))
        {
            $this->setFirst($item);
        }
        else
        {
            $penultimate->setNext($item);
            $item->setPrev($penultimate);
        }

        return $item;
    }

    /**
     * Insert item before $nextItem and maintain continuity
     */
    public function prependItem(ILinkedListItem $nextItem, string $value): ILinkedListItem
    {
        $item = new LinkedListItem($value);
        $item->setNext($nextItem);
        $item->setPrev($nextItem->getPrev());
        if(is_null($nextItem->getPrev()))
        {
            $this->setFirst($item);
        }
        else
        {
            $nextItem->getPrev()->setNext($item);
        }
        $nextItem->setPrev($item);

        return $item;
    }

    /**
     * Insert item after $prevItem and maintain continuity
     */
    public function appendItem(ILinkedListItem $prevItem, string $value): ILinkedListItem
    {
        $item = new LinkedListItem($value);
        $item->setPrev($prevItem);
        $item->setNext($prevItem->getNext());
        if(is_null($prevItem->getNext()))
        {
            $this->setLast($item);
        }
        else
        {
            $prevItem->getNext()->setPrev($item);
        }
        $prevItem->setNext($item);

        return $item;
    }
}
