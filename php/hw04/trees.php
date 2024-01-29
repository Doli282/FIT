<?php declare(strict_types=1);

use Tester\TreeTestHelper;

global $trees;
$trees = [];

/// ======================= ///
/// Tries ids stars from 0. ///
/// ======================= ///
/**
             1
            /\
           2  3
          /\
         4  5
*/
$trees[] = new TreeTestHelper(
    (new Node(1))
        ->setLeft((new Node(2))
            ->setLeft(new Node(4))
            ->setRight(new Node(5))
        )
        ->setRight(new Node(3)),
    '4 2 5 1 3',
    '1 2 4 5 3',
    '4 5 2 3 1'
);


/**

                   10
                 /   \
                5    3
               /\   /
              2 4  5
             /    / \
            1    7  12

*/
$trees[] = new TreeTestHelper(
    (new Node(10))
        ->setLeft((new Node(5))
            ->setLeft((new Node(2))
                ->setLeft(new Node(1))
            )
            ->setRight(new Node(4))
        )
        ->setRight((new Node(3))
            ->setLeft((new Node(5))
                ->setLeft(new Node(7))
                ->setRight(new Node(12))
            )
    ),'1 2 5 4 10 7 5 12 3',
    '10 5 2 1 4 3 5 7 12',
    '1 2 4 5 7 12 5 3 10'
);


/**

 2

*/

$trees[] = new TreeTestHelper(
    new Node(2),
    '2',
    '2',
    '2'
);


/**

                1
         /            \
       2               3
    /    \          /     \
  4       5        6      7
 / \    /  \     /  \    /  \
8  9   10  11   12  13  14  15

*/
$trees[] = new TreeTestHelper(
    (new Node(1))
        ->setLeft((new Node(2))
            ->setLeft((new Node(4))
                ->setLeft(new Node(8))
                ->setRight(new Node(9))
            )
            ->setRight((new Node(5))
                ->setLeft(new Node(10))
                ->setRight(new Node(11))
            )
        )
        ->setRight((new Node(3))
            ->setLeft((new Node(6))
                ->setLeft(new Node(12))
                ->setRight(new Node(13))
            )
            ->setRight((new Node(7))
                ->setLeft(new Node(14))
                ->setRight(new Node(15))
            )
        ),
    '8 4 9 2 10 5 11 1 12 6 13 3 14 7 15',
    '1 2 4 8 9 5 10 11 3 6 12 13 7 14 15',
    '8 9 4 10 11 5 2 12 13 6 14 15 7 3 1'
);

/**

 2
  \
   4
    \
     8
      \
      16
        \
        32

*/
$trees[] = new TreeTestHelper(
    (new Node(2))
        ->setRight((new Node(4))
            ->setRight((new Node(8))
                ->setRight((new Node(16))
                    ->setRight(new Node(32))
                )
            )
        ),
    '2 4 8 16 32',
    '2 4 8 16 32',
    '32 16 8 4 2'
);
