<?php declare(strict_types=1);

require_once('./autoloader.php');
require_once('./trees.php');

// Don't do anything with the following code, it is just for testing your solution.

// Test autoloader
echo "*> Autoloader ..." . PHP_EOL;
$class = 'Iterator\\InOrderIterator';
echo '=> ' . (class_exists($class) ? 'OK [1]' : 'FAILED') . PHP_EOL;



global $trees;
/** @var Tester\TreeTestHelper $tree */
$tree = $trees[0];

// Test in-order iterator
echo PHP_EOL . "*> In-Order Iterator ..." . PHP_EOL;
$iter = new Iterator\InOrderIterator($tree->getRoot());
$inOrderOk = $tree->treeToString($iter) === $tree->getIn();
echo '=> ' . ($inOrderOk ? 'OK [1]' : 'FAILED') . PHP_EOL;


// Test pre- and post-order iterators
echo PHP_EOL . "*> Pre- and Post-Order Iterators ..." . PHP_EOL;
$iter = new Iterator\PreOrderIterator($tree->getRoot());
$preOrderOk = $tree->treeToString($iter) === $tree->getPre();
$iter = new Iterator\PostOrderIterator($tree->getRoot());
$postOrderOk = $tree->treeToString($iter) === $tree->getPost();
echo '=> ' . ($preOrderOk && $postOrderOk ? 'OK [2]' : 'FAILED') . PHP_EOL;

