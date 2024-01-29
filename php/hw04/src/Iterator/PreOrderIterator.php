<?php declare(strict_types=1);

namespace Iterator;

use Node;

class PreOrderIterator extends AbstractOrderIterator
{
    // TODO specific methods
    protected function createStack(Node $currentNode) : void
    {
        if(is_null($currentNode))
        {
            return;
        }
        $workingStack = array();
        while(count($workingStack) > 0 || !is_null($currentNode))
        {
            if(!is_null($currentNode))
            {
                $this->stack[] = $currentNode;

                if(!is_null($currentNode->getRight()))
                {
                    $workingStack[] = $currentNode->getRight();
                }
                $currentNode = $currentNode->getLeft();
            }
            else
            {
                $currentNode = array_pop($workingStack);
            }
        }
    }
}
