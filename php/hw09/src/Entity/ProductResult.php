<?php declare(strict_types=1);

namespace BIPHP\Entity;

class ProductResult
{
    private string $name;
    private string $description;
    private ?float $totalPriceWithoutDiscount = null;
    private float $totalPrice;
    private string $link;

    /**
     * @param array<RatingResult> $ratings
     */
    public function __construct(
        private array $ratings = [],
        private ?int $id = null
    )
    {
    }

    /**
     * @return RatingResult[]
     */
    public function getRatings(): array
    {
        return $this->ratings;
    }

    public function addRating(RatingResult $rating): void
    {
        $this->ratings[] = $rating;
    }



    public function getName(): string
    {
        return $this->name;
    }


    public function setName(string $name): void
    {
        $this->name = $name;
    }


    public function getDescription(): string
    {
        return $this->description;
    }


    public function setDescription(string $description): void
    {
        $this->description = $description;
    }


    public function getTotalPriceWithoutDiscount(): ?float
    {
        return $this->totalPriceWithoutDiscount;
    }

    /**
     * @param float|null $totalPriceWithoutDiscount
     */
    public function setTotalPriceWithoutDiscount(?float $totalPriceWithoutDiscount): void
    {
        $this->totalPriceWithoutDiscount = $totalPriceWithoutDiscount;
    }

    public function getTotalPrice(): float
    {
        return $this->totalPrice;
    }

    public function setTotalPrice(float $totalPrice): void
    {
        $this->totalPrice = $totalPrice;
    }

    public function getLink(): string
    {
        return $this->link;
    }

    public function setLink(string $link): void
    {
        $this->link = $link;
    }

    public function getId(): int
    {
        return $this->id;
    }

    public function setId(int $id): void
    {
        $this->id = $id;
    }
}
