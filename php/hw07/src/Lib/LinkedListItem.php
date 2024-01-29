<?php declare(strict_types=1);

namespace HW\Lib;

use HW\Interfaces\ILinkedListItem;

class LinkedListItem implements ILinkedListItem
{
    protected string $value;

    protected ?ILinkedListItem $next = null;

    protected ?ILinkedListItem $prev = null;

    /**
     * LinkedListItem constructor.
     */
    public function __construct(string $value)
    {
        $this->value = $value;
    }

    public function getValue(): string
    {
        return $this->value;
    }

    public function setValue(string $value): ILinkedListItem
    {
        $this->value = $value;
        return $this;
    }

    public function getNext(): ?ILinkedListItem
    {
        return $this->next;
    }

    public function setNext(?ILinkedListItem $next): ILinkedListItem
    {
        $this->next = $next;
        return $this;
    }

    public function getPrev(): ?ILinkedListItem
    {
        return $this->prev;
    }

    public function setPrev(?ILinkedListItem $prev): ILinkedListItem
    {
        $this->prev = $prev;
        return $this;
    }
}
