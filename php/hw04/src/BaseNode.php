<?php declare(strict_types=1);

abstract class BaseNode
{
    protected ?Node $left = null;
    protected ?Node $right = null;

    public function __construct(protected int $value)
    {
    }

    final public function getValue(): int
    {
        return $this->value;
    }

    final public function getLeft(): ?Node
    {
        return $this->left;
    }

    final public function setLeft(?Node $left): BaseNode
    {
        $this->left = $left;
        return $this;
    }

    final public function getRight(): ?Node
    {
        return $this->right;
    }

    final public function setRight(?Node $right): BaseNode
    {
        $this->right = $right;
        return $this;
    }
}
