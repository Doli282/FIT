<?php declare(strict_types=1);

namespace BIPHP\Entity;

class RatingResult
{
    public function __construct(
        private string $username,
        private string $comment,
        private int $rating,
    )
    {
    }

    public function getUsername(): string
    {
        return $this->username;
    }

    public function setUsername(string $username): void
    {
        $this->username = $username;
    }

    public function getComment(): string
    {
        return $this->comment;
    }

    public function setComment(string $comment): void
    {
        $this->comment = $comment;
    }

    public function getRating(): int
    {
        return $this->rating;
    }

    public function setRating(int $rating): void
    {
        $this->rating = $rating;
    }
}
