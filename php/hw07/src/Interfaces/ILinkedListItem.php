<?php

namespace HW\Interfaces;

interface ILinkedListItem
{
    public function __construct(string $value);

    public function getValue(): string;

    public function setValue(string $value): ILinkedListItem;

    public function getNext(): ?ILinkedListItem;

    public function setNext(?ILinkedListItem $next): ILinkedListItem;

    public function getPrev(): ?ILinkedListItem;

    public function setPrev(?ILinkedListItem $prev): ILinkedListItem;
}