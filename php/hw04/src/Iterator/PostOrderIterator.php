<?php declare(strict_types=1);

namespace Iterator;

use Node;

class PostOrderIterator extends AbstractOrderIterator
{
    // TODO specific methods
    protected function createStack(Node $currentNode) : void
    {
        if (is_null($currentNode)) {
            return;
        }
        $workingStack = array();
        $prevNode = null;
        $root = $currentNode;

        while (count($workingStack) > 0 || !is_null($root)) {
            if(!is_null($root))
            {
                $workingStack[] = $root;
                $root = $root->getLeft();
            }
            else
            {
                $currentNode = $workingStack[count($workingStack) - 1];
                if(!is_null($currentNode->getRight()) && $currentNode->getRight() !== $prevNode)
                {
                    $root =$currentNode->getRight();
                }
                else {
                    $this->stack[] = $currentNode;
                    $prevNode = $currentNode;
                    array_pop($workingStack);
                }
            }
        }
    }
}
