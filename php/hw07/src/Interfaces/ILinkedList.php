<?php

namespace HW\Interfaces;

interface ILinkedList
{
    /**
     * gets first item from list, returns null if empty
     * @return ILinkedListItem|null
     */
    public function getFirst(): ?ILinkedListItem;

    /**
     * gets last item from list, returns null if empty
     * @return ILinkedListItem|null
     */
    public function getLast(): ?ILinkedListItem;

    /**
     * Place new item at the begining of the list
     */
    public function prependList(string $value): ILinkedListItem;

    /**
     * Place new item at the end of the list
     */
    public function appendList(string $value): ILinkedListItem;

    /**
     * Insert item before $nextItem and maintain continuity
     */
    public function prependItem(ILinkedListItem $nextItem, string $value): ILinkedListItem;

    /**
     * Insert item after $prevItem and maintain continuity
     */
    public function appendItem(ILinkedListItem $prevItem, string $value): ILinkedListItem;
}