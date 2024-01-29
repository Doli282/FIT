<?php declare(strict_types=1);

namespace HW\Tests;

use HW\Factory\LinkedListFactory;
use HW\Lib\LinkedList;
use HW\Lib\LinkedListItem;
use PHPUnit\Framework\TestCase;

class LinkedListItemTest extends TestCase
{
    /** @var LinkedList $list  */
    protected $list;

    public function setUp(): void
    {
        parent::setUp();
        $this->list = LinkedListFactory::get();
    }


    public function testValue() : void
    {
        $value = "0";
        $item = new LinkedListItem($value);
        $this->assertEquals($value, $item->getValue());
        $value2 = "5";
        $item->setValue($value2);
        $this->assertEquals($value2, $item->getValue());
    }

    public function testNext() : void
    {
        $value = "1";
        $value2 = "2";
        $itemPrev = new LinkedListItem($value);
        $itemNext = new LinkedListItem(($value2));

        $this->assertNull($itemPrev->getNext());
        $itemPrev->setNext($itemNext);
        $this->assertEquals($itemNext, $itemPrev->getNext());
    }

    public function testPrev() : void
    {
        $value = "1";
        $value2 = "2";
        $itemPrev = new LinkedListItem($value);
        $itemNext = new LinkedListItem(($value2));

        $this->assertNull($itemNext->getPrev());
        $itemNext->setPrev($itemPrev);
        $this->assertEquals($itemPrev, $itemNext->getPrev());
    }

    public function testFirst() : void
    {
        $this->assertNull($this->list->getFirst());
        $item = new LinkedListItem("5");
        $res = $this->list->setFirst($item);
        $this->assertEquals($item, $this->list->getFirst());
        $this->assertEquals($this->list, $res);
    }

    public function testLast() : void
    {
        $this->assertNull($this->list->getLast());
        $item = new LinkedListItem("5");
        $res = $this->list->setLast($item);
        $this->assertEquals($item, $this->list->getLast());
        $this->assertEquals($this->list, $res);
    }

    /** @depends testFirst
     *  @depends testLast
     *  @depends testPrev
     *  @depends testNext
     */
    public function testPrependList() : void
    {
        $itemInList = new LinkedListItem("1");
        $this->list->setFirst($itemInList);
        $this->list->setLast($itemInList);

        $value = "0";
        $newItem = $this->list->prependList($value);

        $this->assertNotNull($newItem);
        $this->assertEquals($value, $newItem->getValue());
        $this->assertEquals($this->list->getFirst(), $newItem);
        $this->assertNull($newItem->getPrev());
        $this->assertEquals($itemInList, $newItem->getNext());
        $this->assertEquals($newItem, $itemInList->getPrev());
    }

    /** @depends testFirst
     *  @depends testLast
     *  @depends testPrev
     *  @depends testNext
     */
    public function testPrependListEmpty() : void
    {
        $value = "0";
        $newItem = $this->list->prependList($value);

        $this->assertNotNull($newItem);
        $this->assertEquals($value, $newItem->getValue());
        $this->assertNull($newItem->getPrev());
        $this->assertNull($newItem->getNext());
        $this->assertEquals($newItem, $this->list->getFirst());
        $this->assertEquals($newItem, $this->list->getLast());
    }

    /** @depends testLast
     *  @depends testFirst
     *  @depends testPrev
     *  @depends testNext
     */
    public function testAppendList() : void
    {
        $itemInList = new LinkedListItem("1");
        $this->list->setFirst($itemInList);
        $this->list->setLast($itemInList);

        $value = "2";
        $newItem = $this->list->appendList($value);

        $this->assertNotNull($newItem);
        $this->assertEquals($value, $newItem->getValue());
        $this->assertEquals($newItem, $this->list->getLast());
        $this->assertNull($newItem->getNext());
        $this->assertEquals($itemInList, $newItem->getPrev());
        $this->assertEquals($newItem, $itemInList->getNext());
    }

    /** @depends testFirst
     *  @depends testLast
     *  @depends testPrev
     *  @depends testNext
     */
    public function testAppendListEmpty() : void
    {
        $value = "0";
        $newItem = $this->list->appendList($value);

        $this->assertNotNull($newItem);
        $this->assertEquals($value, $newItem->getValue());
        $this->assertNull($newItem->getPrev());
        $this->assertNull($newItem->getNext());
        $this->assertEquals($newItem, $this->list->getLast());
        $this->assertEquals($newItem, $this->list->getFirst());
    }

    /** @depends testPrependList
     *  @depends testPrependListEmpty
     *  @depends testPrev
     *  @depends testNext
     */
    public function testPrependItemMiddle() : void
    {
        $lastItem = $this->list->prependList("2");
        $firstItem = $this->list->prependList("0");

        $value = "1";
        $newItem = $this->list->prependItem($lastItem, $value);

        $this->assertNotNull($newItem);
        $this->assertEquals($value, $newItem->getValue());
        $this->assertEquals($firstItem, $newItem->getPrev());
        $this->assertEquals($lastItem, $newItem->getNext());
        $this->assertEquals($newItem, $firstItem->getNext());
        $this->assertEquals($newItem, $lastItem->getPrev());
    }

    /** @depends testPrependListEmpty
     *  @depends testFirst
     *  @depends testPrev
     *  @depends testNext
     */
    public function testPrependItemFirst() : void
    {
        $lastItem = $this->list->prependList("2");

        $value = "1";
        $newItem = $this->list->prependItem($lastItem, $value);

        $this->assertNotNull($newItem);
        $this->assertEquals($value, $newItem->getValue());
        $this->assertNull($newItem->getPrev());
        $this->assertEquals($lastItem, $newItem->getNext());
        $this->assertEquals($newItem, $lastItem->getPrev());
        $this->assertEquals($newItem, $this->list->getFirst());
    }

    /** @depends testAppendList
     *  @depends testAppendListEmpty
     *  @depends testPrev
     *  @depends testNext
     */
    public function testAppendItemMiddle() : void
    {
        $firstItem = $this->list->appendList("0");
        $lastItem = $this->list->appendList("2");

        $value = "1";
        $newItem = $this->list->appendItem($firstItem, $value);

        $this->assertNotNull($newItem);
        $this->assertEquals($value, $newItem->getValue());
        $this->assertEquals($firstItem, $newItem->getPrev());
        $this->assertEquals($lastItem, $newItem->getNext());
        $this->assertEquals($newItem, $firstItem->getNext());
        $this->assertEquals($newItem, $lastItem->getPrev());
    }

    /** @depends testAppendListEmpty
     *  @depends testLast
     *  @depends testPrev
     *  @depends testNext
     */
    public function testAppendItemLast() : void
    {
        $firstItem = $this->list->appendList("0");

        $value = "1";
        $newItem = $this->list->appendItem($firstItem, $value);

        $this->assertNotNull($newItem);
        $this->assertEquals($value, $newItem->getValue());
        $this->assertEquals($firstItem, $newItem->getPrev());
        $this->assertNull($newItem->getNext());
        $this->assertEquals($newItem, $firstItem->getNext());
        $this->assertEquals($newItem, $this->list->getLast());
    }
}
