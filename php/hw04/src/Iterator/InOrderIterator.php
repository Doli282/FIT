<?php declare(strict_types=1);

namespace Iterator;

use Node;

class InOrderIterator extends AbstractOrderIterator
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
                $workingStack[] = $currentNode;
                $currentNode = $currentNode->getLeft();
            }
            else
            {
             $currentNode = array_pop($workingStack);
             $this->stack[] = $currentNode;
             $currentNode = $currentNode->getRight();
            }
        }
    }
}
