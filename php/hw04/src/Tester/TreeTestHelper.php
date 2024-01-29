<?php declare(strict_types=1);

namespace Tester;
use Iterator\AbstractOrderIterator;
use Node;

final class TreeTestHelper
{
    static int $generator = 0;
    private int $id;
    public function __construct(
        private Node $root,
        private string $in,
        private string $pre,
        private string $post
    )
    {
        $this->id = self::$generator++;
    }

    public function getRoot(): Node
    {
        return $this->root;
    }

    public function getIn(): string
    {
        return $this->in;
    }

    public function getPre(): string
    {
        return $this->pre;
    }

    public function getPost(): string
    {
        return $this->post;
    }

    public function getId(): int
    {
        return $this->id;
    }

    public function treeToString(AbstractOrderIterator $iterator): string
    {
        $values = [];
        foreach ($iterator as $node) {
            $values[] = $node->getValue();
        }
        return implode(' ', $values);
    }
}
