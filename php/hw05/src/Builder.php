<?php declare(strict_types=1);

namespace App;

use App\Invoice\Address;
use App\Invoice\BusinessEntity;
use App\Invoice\Item;

class Builder
{
    protected Invoice $invoice;

    public function __construct()
    {
        $this->invoice = new Invoice();
    }
    public function build(): Invoice
    {
        // TODO implement - done
        return clone $this->invoice;
    }

    public function setNumber(string $number): self
    {
        // TODO implement - done
        $this->invoice->setNumber($number);
        return $this;
    }

    public function setSupplier(
        string  $name,
        string  $vatNumber,
        string  $street,
        string  $number,
        string  $city,
        string  $zip,
        ?string $phone = null,
        ?string $email = null
    ): self
    {
        // TODO implement - done
        $address = new Address();
        $address->setStreet($street)->setNumber($number)
            ->setCity($city)->setZipCode($zip)
            ->setPhone($phone)->setEmail($email);
        $supplier = new BusinessEntity();
        $supplier->setName($name)->setVatNumber($vatNumber)->setAddress($address);
        $this->invoice->setSupplier($supplier);
        return $this;
    }

    public function setCustomer(
        string  $name,
        string  $vatNumber,
        string  $street,
        string  $number,
        string  $city,
        string  $zip,
        ?string $phone = null,
        ?string $email = null
    ): self
    {
        // TODO implement - done
        $address = new Address();
        $address->setStreet($street)->setNumber($number)
            ->setCity($city)->setZipCode($zip)
            ->setPhone($phone)->setEmail($email);
        $customer = new BusinessEntity();
        $customer->setName($name)->setVatNumber($vatNumber)->setAddress($address);
        $this->invoice->setCustomer($customer);
        return $this;
    }

    public function addItem(string $description, ?float $quantity, ?float $price): self
    {
        // TODO implement - done
        $item = new Item();
        $item->setDescription($description)->setQuantity($quantity)->setUnitPrice($price);
        $this->invoice->addItem($item);
        return $this;
    }
}
