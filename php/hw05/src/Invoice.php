<?php declare(strict_types=1);

namespace App;

use App\Invoice\BusinessEntity;
use App\Invoice\Item;

class Invoice
{
    protected string $number;

    protected BusinessEntity $supplier;

    protected BusinessEntity $customer;

    /** @var Item[]|array */
    protected array $items;

    public function __construct()
    {
        $this->customer = new BusinessEntity();
        $this->supplier = new BusinessEntity();
        $this->number = "";
    }


    public function getNumber(): string
    {
        return $this->number;
    }

    public function setNumber(string $number): Invoice
    {
        $this->number = $number;
        return $this;
    }

    public function getSupplier(): BusinessEntity
    {
        return $this->supplier;
    }

    public function setSupplier(BusinessEntity $supplier): Invoice
    {
        $this->supplier = $supplier;
        return $this;
    }

    public function getCustomer(): BusinessEntity
    {
        return $this->customer;
    }

    public function setCustomer(BusinessEntity $customer): Invoice
    {
        $this->customer = $customer;
        return $this;
    }

    /**
     * @return Item[]
     */
    public function getItems(): array
    {
        return $this->items;
    }

    public function addItem(Item $item): self
    {
        $this->items[] = $item;
        return $this;
    }

    public function getTotalPrice(): float|int
    {
        // TODO implement - done
        $sum = 0;
        foreach ($this->items as $item)
        {
            $sum += $item->getTotalPrice();
        }
        return $sum;
    }
}
