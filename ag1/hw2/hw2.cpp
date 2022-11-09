#ifndef __PROGTEST__
#include <cassert>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <algorithm>
#include <bitset>
#include <list>
#include <array>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <random>

#endif

// TODO implement
template <typename Product>
struct Bestsellers
{
public:
  // The total number of tracked products
  size_t products() const
  {
    return productsNO;
  }

  void sell(const Product &p, size_t amount)
  {
    std::shared_ptr<Node> newProduct = std::make_shared<Node>(amount, p); // new possible Product
    if (productsDB.emplace(p, newProduct).second)                         // if product insert successful -> was not in the table
    {
      productsNO++;
      addToTree(newProduct, amount);
    }
    else
    {
      addSale(p, amount);
    }
    return;
  }

  // The most sold product has rank 1
  size_t rank(const Product &p) const
  {
    // size_t key;
    std::shared_ptr<Node> node;
    try
    {
      node = productsDB.at(p);
    }
    catch (...)
    {
      // product is not in the DB
      throw std::out_of_range("");
    }

    size_t overallRank = node->rankInSubtree;
    std::shared_ptr<Node> parent = node->parent.lock();
    while (parent) // search tree from root to the leafs
    {
      if (isLeftChild(node)) // node is left child
      {
        overallRank += parent->rankInSubtree; // save rank in the subtree to the overall rank
      }
      node = parent;
      parent = parent->parent.lock();
    }
    return overallRank;
  }

  const Product &product(size_t rank) const
  {
    return findNodeByRank(rank)->productID;
  }

  // How many copies of product with given rank were sold
  size_t sold(size_t rank) const
  {
    return findNodeByRank(rank)->amountSold;
  }
  // The same but sum over interval of products (including from and to)
  // It must hold: sold(x) == sold(x, x)
  size_t sold(size_t from, size_t to) const
  {
    if (from == 0 || from > to || to == 0 || to > productsNO)
      throw std::out_of_range("");
    std::shared_ptr<Node> node = root;
    size_t rankInSubtree;
    while (node)
    {
      rankInSubtree = node->rankInSubtree;
      if (rankInSubtree > to)
      {
        node = node->rightNode;
      }
      else if (rankInSubtree < from)
      {
        from -= rankInSubtree; // adjust rank to the current subtree
        to -= rankInSubtree;   // adjust rank to the current subtree
        node = node->leftNode;
      }
      else
      { // rank found
        break;
      }
    }
    return node->amountSoldBelow + amountInIntervalMAX(node, to) + amountInIntervalMIN(node, from);
  }

  // Bonus only, ignore if you are not interested in bonus
  // The smallest (resp. largest) rank with sold(rank) == sold(r)
  size_t first_same(size_t r) const { return 0; }
  size_t last_same(size_t r) const { return 0; }

  /* void showTree()
  {
    std::cout << "=== TREE ===" << std::endl;
    // treeShow(root, "", false);
    treeShow(root, -1);
    std::cout << "=== TREE END ===" << std::endl;
    // DBShow();
    std::cout << std::endl;
    return;
  } */

private:
  struct Node
  {
    Node(size_t amount, const Product &productName) : amountSold(amount), amountSoldBelow(amount), productID(productName) {}
    int delta = 0;
    size_t rankInSubtree = 1;
    size_t amountSold;
    size_t amountSoldBelow;
    std::weak_ptr<Node> parent;
    std::shared_ptr<Node> rightNode;
    std::shared_ptr<Node> leftNode;
    // std::shared_ptr<Node> followingNode;
    Product productID;
  };

  std::shared_ptr<Node> root = nullptr;
  size_t productsNO = 0;
  std::unordered_map<Product, std::shared_ptr<Node>> productsDB;

  size_t setAmountSoldBelow(std::shared_ptr<Node> x)
  {
    return x->amountSoldBelow = ((x->leftNode) ? x->leftNode->amountSoldBelow : 0) + ((x->rightNode) ? x->rightNode->amountSoldBelow : 0) + x->amountSold;
  }
  // check if child is a left child -> child without parent IS a left child
  bool isLeftChild(std::shared_ptr<Node> child) const noexcept
  {
    std::shared_ptr<Node> parent = child->parent.lock();
    if (parent)
    {
      // bool left = (child == parent->leftNode);
      // std::cout << child->productID << " is left child of " << parent->productID << left << std::endl;
      return (child == parent->leftNode);
    }
    else
    {
      return true;
    }
  }

  void setChild(std::shared_ptr<Node> adoptiveParent, std::shared_ptr<Node> child, bool isLeftChild)
  {
    if (adoptiveParent)
    {
      if (isLeftChild) // set child as left or right accordingly
      {
        adoptiveParent->leftNode = child;
      }
      else
      {
        adoptiveParent->rightNode = child;
      }
    }
    else
    { // parent does not exist -> it was root
      root = child;
    }
    if (child)
    {
      child->parent = adoptiveParent; // child has new parent
    }
  }

  std::shared_ptr<Node> rotateRight(std::shared_ptr<Node> x)
  {
    std::shared_ptr<Node> y = x->leftNode;
    std::shared_ptr<Node> treeB = y->rightNode;
    setChild(x->parent.lock(), y, isLeftChild(x));
    setChild(x, treeB, true);
    setChild(y, x, false);
    y->rankInSubtree += x->rankInSubtree; // change ranking of nodes
    setAmountSoldBelow(x);                // adjust amount sold below
    setAmountSoldBelow(y);
    x->delta = (++(y->delta)) ? -1 : 0; // alter delta accordingly to(from) Y=+1(0) && X=-1(-2) || Y=0(-1) && X=0(-2)
    return y;
  }
  std::shared_ptr<Node> rotateLeft(std::shared_ptr<Node> x)
  {
    std::shared_ptr<Node> y = x->rightNode;
    std::shared_ptr<Node> treeB = y->leftNode;
    setChild(x->parent.lock(), y, isLeftChild(x));
    setChild(x, treeB, false);
    setChild(y, x, true);
    x->rankInSubtree -= y->rankInSubtree; // change ranking of nodes
    setAmountSoldBelow(x);                // adjust amount sold below
    setAmountSoldBelow(y);
    x->delta = (--(y->delta)) ? +1 : 0; // alter delta accordingly to(from) Y=-1(0) && X=+1(+2) || Y=0(+1) && X=0(+2)
    return y;
  }
  std::shared_ptr<Node> rotateRightLeft(std::shared_ptr<Node> x) // double rotation to the left -> first to the right, then to the left
  {
    std::shared_ptr<Node> y = x->rightNode;
    std::shared_ptr<Node> z = y->leftNode;
    //std::cout << (z ? "z neexistuje" : "z v poradku") << std::endl;
    //std::cout << "z=" << z->delta << ((z->leftNode) ? "levy" : "null") << " "
    //<< ((z->rightNode) ? "pravy" : "null") << std::endl;
    std::shared_ptr<Node> treeB = z->leftNode;
    std::shared_ptr<Node> treeC = z->rightNode;
    setChild(x->parent.lock(), z, isLeftChild(x));
    setChild(x, treeB, false);
    setChild(y, treeC, true);
    setChild(z, x, true);
    setChild(z, y, false);

    x->rankInSubtree -= (z->rankInSubtree += y->rankInSubtree);                         // adjust ranking
    z->amountSoldBelow = setAmountSoldBelow(x) + setAmountSoldBelow(y) + z->amountSold; // adjust amounts below

    if (z->delta == +1)
    {
      x->delta = -1;
      y->delta = 0;
    }
    else if (z->delta == -1)
    {
      x->delta = 0;
      y->delta = +1;
    }
    else
    {
      x->delta = 0;
      y->delta = 0;
    }
    z->delta = 0;
    return z;
  }
  std::shared_ptr<Node> rotateLeftRight(std::shared_ptr<Node> x) // double rotation to the right -> first to the left, then to the right
  {
    std::shared_ptr<Node> y = x->leftNode;
    std::shared_ptr<Node> z = y->rightNode;
    /* if(z)
    {
      std::cout << "z v poradku" << std::endl;
    }
    else
    {
      treeShow(x->parent.lock(),0);
    } */
    //std::cout << "z=" << z->delta << ((z->leftNode) ? "levy" : "null") << " "
    //<< ((z->rightNode) ? "pravy" : "null") << std::endl;
    std::shared_ptr<Node> treeB = z->leftNode;
    std::shared_ptr<Node> treeC = z->rightNode;
    setChild(x->parent.lock(), z, isLeftChild(x));
    setChild(x, treeC, true);
    setChild(y, treeB, false);
    setChild(z, x, false);
    setChild(z, y, true);

    y->rankInSubtree -= z->rankInSubtree; // adjust ranking
    z->rankInSubtree += x->rankInSubtree;
    z->amountSoldBelow = setAmountSoldBelow(x) + setAmountSoldBelow(y) + z->amountSold; // adjust amount sold below

    if (z->delta == 1)
    {
      x->delta = 0;
      y->delta = -1;
    }
    else if (z->delta == -1)
    {
      x->delta = +1;
      y->delta = 0;
    }
    else
    {
      x->delta = 0;
      y->delta = 0;
    }
    z->delta = 0;
    return z;
  }

  void balanceTreeAfterInsert(std::shared_ptr<Node> parent, std::shared_ptr<Node> child)
  {
    int balanceFactor;
    while (parent) // propagate upstream up to the root
    {
      balanceFactor = parent->delta += (child == parent->leftNode) ? (-1) : 1; // -1 for left child, +1 for right child

      if (balanceFactor < -1) // tree is left-heavy
      {
        if (child->delta > 0) // information came from the inside node
        {
          // std::cout << "rotuji vlevo a vpravo" << std::endl;
          rotateLeftRight(parent);
        }
        else
        {
          // std::cout << "rotuji vpravo" << std::endl;
          rotateRight(parent);
        }
        return;
      }
      else if (balanceFactor > 1) // tree is right-heavy
      {
        if (child->delta < 0) // information came from the inside node
        {
          // std::cout << "rotuji vpravo a vlevo" << std::endl;
          rotateRightLeft(parent);
        }
        else
        {
          // std::cout << "rotuji vlevo" << std::endl;
          rotateLeft(parent);
        }
        return;
      }
      else if (balanceFactor == 0)
      { // tree got balanced, stop propagation
        // std::cout << "koncim bez rotace" << std::endl;
        return;
      }

      // move to the upper floor
      // std::cout << "posouvam se vzhuru" << std::endl;
      child = parent;
      parent = parent->parent.lock();
    }
    return;
  }

  bool addToTree(std::shared_ptr<Node> child, const size_t amount)
  {
    if (!root)
    { // check if the root is not nullptr
      root = child;
      return true;
    }
    std::shared_ptr<Node> parent;
    std::shared_ptr<Node> descendant = root;
    while (descendant)
    { // while I am not in a list
      parent = descendant;
      parent->amountSoldBelow += amount;
      if (descendant->amountSold < amount)
      {                                 // if the key of a parent is less than the key of the child
        descendant->rankInSubtree += 1; // there is one more node in front of this node
        descendant = descendant->rightNode;
      }
      else
      {
        descendant = descendant->leftNode;
      }
    }
    if (parent->amountSold < amount)
    { // if the key of a parent is less than the key of the child
      parent->rightNode = child;
    }
    else
    {
      parent->leftNode = child;
    }
    child->parent = parent;
    // propagace zmeny vysky stromu
    balanceTreeAfterInsert(parent, child);
    return true;
  }

  bool balanceTreeAfterDelete(std::shared_ptr<Node> x, bool fromLeft)
  {
    //std::cout << "balancing..." << std:: endl;
    while (x) // iterate while not in root
    {
      setAmountSoldBelow(x);
       //std::cout << "fromLeft = " << fromLeft << std::endl;
       //std::cout << "x=" << x->delta
       //<< ((x->leftNode) ? "levy" : "null") << " "
       //<< ((x->rightNode) ? "pravy" : "null") << std::endl;
      if (fromLeft) // the signal came from left
      {
        if (x->delta == -1)
        {
          x->delta = 0;
          // propagate
        }
        else if (x->delta == 0)
        {
          x->delta = 1;
          // stop propagation
          break;
        }
        else // x->delta == 1
        {
          if (x->rightNode->delta == 1)
          {
            x = rotateLeft(x);
            // propagate
          }
          else if (x->rightNode->delta == 0)
          {
            x = rotateLeft(x);
            // stop propagation
            break;
          }
          else // x->rightNode->delta == -1
          {
            x = rotateRightLeft(x);
            // propagate
          }
        }
      }
      else
      { // from right
        // propagate the information that a node with lower rank was deleted
        x->rankInSubtree -= 1;
        if (x->delta == 1)
        {
          x->delta = 0;
          // propagate
        }
        else if (x->delta == 0)
        {
          x->delta = -1;
          // stop propagation
          break;
        }
        else // x->delta == -1
        {
      //    std::cout << "y=" << x->leftNode->delta << ((x->leftNode->leftNode) ? "levy" : "null") << " "
    //<< ((x->leftNode->rightNode) ? "pravy" : "null") << std::endl;
          if (x->leftNode->delta == -1)
          {
            //std::cout << "rotuji doprava" << std::endl;
            x = rotateRight(x);
            // propagate
          }
          else if (x->leftNode->delta == 0)
          {
            x = rotateRight(x);
            // stop propagation
            break;
          }
          else // x->rightNode->delta == 1
          {
            x = rotateLeftRight(x);
            // propagate
          }
        }
      }
      fromLeft = isLeftChild(x);
      x = x->parent.lock();
    }

    // propagate information about the delete to the root
    if (x)
    {
      fromLeft = isLeftChild(x);
      x = x->parent.lock();
      while (x)
      {
        setAmountSoldBelow(x);
        if (!fromLeft)
        {
          x->rankInSubtree -= 1;
        }
        fromLeft = isLeftChild(x);
        x = x->parent.lock();
      }
    }

    return true;
  }

  bool deleteFromTree(std::shared_ptr<Node> removed)
  {
    //std::cout << "deleting... " << removed->productID  << " " << removed->amountSold << std::endl;
    //std::cout << "removed=" << removed->delta << ((removed->leftNode) ? "levy" : "null") << " "
    //<< ((removed->rightNode) ? "pravy" : "null")
    //<< ((removed->parent.lock()) ? removed->parent.lock()->productID : "null") << std::endl;
    // removed Node is missing a child
    std::shared_ptr<Node> affected = nullptr; // first node affected by a node disappearing
    bool fromLeft;                            // direction of the signal of a node disappearing
    if (!removed->leftNode)
    { // removed node does not have left child
      fromLeft = isLeftChild(removed);
      affected = removed->parent.lock();
      setChild(affected, removed->rightNode, fromLeft);
    }
    else if (!removed->rightNode)
    { // removed node does not have right child
      fromLeft = isLeftChild(removed);
      affected = removed->parent.lock();
      setChild(affected, removed->leftNode, fromLeft);
    }
    else
    { // removed node does have both children
      std::shared_ptr<Node> successor = removed->rightNode;
      std::shared_ptr<Node> nextSuccessor = successor->leftNode;
      if (nextSuccessor)
      { // successor has left child -> find the correct successor
        while (nextSuccessor)
        {
          successor = nextSuccessor;
          nextSuccessor = nextSuccessor->leftNode;
        }
        affected = successor->parent.lock();
        fromLeft = true;
        setChild(affected, successor->rightNode, true);
        setChild(successor, removed->rightNode, false);
      }
      else
      {                       // successor is directly connected to the removed node
        affected = successor; // successor got new left children
        fromLeft = false;     // successor came from right
      }
      setChild(successor, removed->leftNode, true);
      setChild(removed->parent.lock(), successor, isLeftChild(removed));
      // setAmountSoldBelow(successor);
      successor->delta = removed->delta;                 // overwrite the delta with the information from the previous node in the position
      successor->rankInSubtree = removed->rankInSubtree; // give the successor information about the rank of its predecessor
    }
    if(affected)
    {
    //std::cout << "affected=" << affected->productID << " " << affected->delta << ((affected->leftNode) ? "levy" : "null") << " "
    //<< ((affected->rightNode) ? "pravy" : "null") << std::endl;
    }
    balanceTreeAfterDelete(affected, fromLeft); // balance tree after the delete
    // std::cout << "deleted" << std::endl;
    return true;
  }

  // add sale
  void addSale(const Product &p, size_t amount)
  {
    std::shared_ptr<Node> changed = productsDB.at(p);
    deleteFromTree(changed);
    size_t amountSold = changed->amountSold += amount;
    changed->delta = 0; // nulify pointers and erase data from its former life as a node
    changed->rankInSubtree = 1;
    changed->amountSoldBelow = amountSold;
    changed->leftNode = nullptr;
    changed->rightNode = nullptr;
    addToTree(changed, amountSold);
    return;
  }

  std::shared_ptr<Node> findNodeByRank(size_t rank) const
  {
    if (rank == 0 || rank > productsNO)
      throw std::out_of_range("");
    std::shared_ptr<Node> node = root;
    size_t rankInSubtree;
    while (node)
    {
      rankInSubtree = node->rankInSubtree;
      if (rankInSubtree > rank)
      {
        node = node->rightNode;
      }
      else if (rankInSubtree < rank)
      {
        rank -= rankInSubtree; // adjust rank to the current subtree
        node = node->leftNode;
      }
      else
      { // rank found
        return node;
      }
    }
    throw std::out_of_range("");
    return root;
  }

  size_t amountInIntervalMIN(std::shared_ptr<Node> node, size_t minRank) const
  {
    size_t totalAmount = 0;
    size_t rankInSubtree;
    bool insideInterval = true;
    //std::cout << "in interval MIN" << std::endl;
    while (node)
    {
      rankInSubtree = node->rankInSubtree;
      //std::cout << "iteration: (rankInSubtree):(minRank) " << rankInSubtree << ":" << minRank << std::endl;
      //std::cout << "new node is ";
      if (rankInSubtree < minRank) // am i out of the interval ?
      {
        //std::cout << " leftnode." << "was in interval ? " << insideInterval << std::endl;
        if (insideInterval) // was i before in the interval ?
        {
          totalAmount -= node->amountSoldBelow; // decrease the amount
          insideInterval = false;
        }
        minRank -= rankInSubtree;
        node = node->leftNode; // i must return to the interval
      }
      else if (rankInSubtree >= minRank) // am i in the interval
      {
        //std::cout << " rightnode." << "was in interval ? " << insideInterval << std::endl;
        if (!insideInterval) // was i in the interval before?
        {
          totalAmount += node->amountSoldBelow; // increase amount for returning in the interval
          insideInterval = true;
        }

        node = node->rightNode; // i must try to get out of the interval
      }
    }
    return totalAmount;
  }
  size_t amountInIntervalMAX(std::shared_ptr<Node> node, size_t maxRank) const
  {
    size_t totalAmount = 0;
    size_t rankInSubtree;
    bool insideInterval = true;
    //std::cout << "in interval MAX" << std::endl;
    while (node)
    {
      rankInSubtree = node->rankInSubtree;
      //std::cout << "iteration: (rankInSubtree):(minRank) " << rankInSubtree << ":" << maxRank << std::endl;
      //std::cout << "new node is ";
      if (rankInSubtree > maxRank) // am i out of the interval ?
      {
        //std::cout << " rightnode." << "was in interval ? " << insideInterval << std::endl;
        if (insideInterval) // was i before in the interval ?
        {
          totalAmount -= node->amountSoldBelow; // decrease the amount
          insideInterval = false;
        }

        node = node->rightNode; // i must return to the interval
      }
      else if (rankInSubtree <= maxRank) // am i in the interval
      {
        //std::cout << " leftnode." << "was in interval ? " << insideInterval << std::endl;
        if (!insideInterval) // was i in the interval before?
        {
          totalAmount += node->amountSoldBelow; // increase amount for returning in the interval
          insideInterval = true;
        }
        maxRank -= rankInSubtree;
        node = node->leftNode; // i must try to get out of the interval
      }
    }
    return totalAmount;
  }

  /* void treeShow(std::shared_ptr<Node> node, int prefix)
  {
    if (node)
    {
      prefix++;
      treeShow(node->rightNode, prefix);
      for (int i = 0; i < prefix; i++)
        std::cout << "       ";
      std::cout << node->amountSold << "<" << node->amountSoldBelow << ">[" << node->delta << "]#" << node->rankInSubtree << "(" << (node->parent.lock() ? std::to_string(node->parent.lock()->amountSold) : "root") << "){" << node->productID << "}" << std::endl;
      treeShow(node->leftNode, prefix);
    }
    return;
  }
  void DBShow()
  {
    std::cout << "+++ Product DB +++" << std::endl;
    for (auto &it : productsDB)
    {
      std::cout << it.first << " : " << it.second->amountSold << std::endl;
    }
    std::cout << "+++ +++ +++ +++ +++" << std::endl;
  } */
};

#ifndef __PROGTEST__

void test1()
{
  std::cout << "TEST 1 -> prepared test" << std::endl;
  Bestsellers<std::string> T;
  T.sell("coke", 32);
  T.sell("bread", 1);
  assert(T.products() == 2);
  T.sell("ham", 2);
  T.sell("mushrooms", 12);

  //T.showTree();
  assert(T.products() == 4);
  //std::cout << "pocet produktu OK" << std::endl;
  assert(T.rank("ham") == 3);
  //std::cout << "rank produktu OK" << std::endl;
  assert(T.rank("coke") == 1);
  //std::cout << "rank produktu OK" << std::endl;
  assert(T.sold(1, 3) == 46);
  assert(T.product(2) == "mushrooms");
  //std::cout << "rank OK" << std::endl;

  T.sell("ham", 11);
  //T.showTree();
  assert(T.products() == 4);
  assert(T.product(2) == "ham");
  assert(T.sold(2) == 13);
  assert(T.sold(2, 2) == 13);
  assert(T.sold(1, 2) == 45);
  //T.showTree();
}

void test2()
{
  
  std::cout << "TEST 2 -> prepared test" << std::endl;
    #define CATCH(expr)                 \
    try                               \
    {                                 \
      expr;                           \
      assert(0);                      \
    }                                 \
    catch (const std::out_of_range &) \
    {                                 \
      assert(1);                      \
    };

    Bestsellers<std::string> T;
    T.sell("coke", 32);
    T.sell("bread", 1);

    CATCH(T.rank("ham"));
    CATCH(T.product(3));
    CATCH(T.sold(0));
    CATCH(T.sold(9));
    CATCH(T.sold(0, 1));
    CATCH(T.sold(3, 2));
    CATCH(T.sold(1, 9));

  #undef CATCH 
}

void test0()
{
  std::cout << "TEST 0 -> basic test" << std::endl;
  Bestsellers<std::string> T;
  T.sell("coke", 12);
  //T.showTree();
  T.sell("bread", 1);
  assert(T.products() == 2);
  //T.showTree();
  T.sell("ham", 2);
  //T.showTree();
  T.sell("mushrooms", 32);
  //T.showTree();
}
void test3()
{
  std::cout << "TEST 3 -> increasing series" << std::endl;
  Bestsellers<std::string> T;
  T.sell("coke", 10);
  //T.showTree();
  T.sell("bread", 20);
  //T.showTree();
  assert(T.products() == 2);
  T.sell("ham", 30);
  //T.showTree();
  T.sell("mushrooms", 40);
  //T.showTree();
  T.sell("a", 50);
  //T.showTree();
  T.sell("b", 60);
  T.sell("c", 70);
  T.sell("d", 80);
  //T.showTree();
}
void test4()
{
  std::cout << "TEST 4 -> decreasing series" << std::endl;
  Bestsellers<std::string> T;
  T.sell("coke", 80);
  T.sell("bread", 70);
  assert(T.products() == 2);
  T.sell("ham", 60);
  T.sell("mushrooms", 50);
  // T.showTree();
  T.sell("a", 40);
  T.sell("b", 30);
  T.sell("c", 20);
  T.sell("d", 10);
  //T.showTree();
}
void test5()
{
  std::cout << "TEST 5 -> same numbrers" << std::endl;
  Bestsellers<std::string> T;
  T.sell("coke", 80);
  T.sell("bread", 70);
  assert(T.products() == 2);
  T.sell("ham", 60);
  T.sell("mushrooms", 60);
  // T.showTree();
  T.sell("a", 40);
  T.sell("b", 30);
  T.sell("c", 40);
  T.sell("d", 60);
  //T.showTree();
}
void test6()
{
  std::cout << "TEST 6 -> sell same products" << std::endl;
  Bestsellers<std::string> T;
  T.sell("coke", 80);
  T.sell("bread", 70);
  assert(T.products() == 2);
  T.sell("ham", 60);
  T.sell("mushrooms", 50);
  // T.showTree();
  T.sell("a", 40);
  T.sell("b", 30);
  T.sell("c", 20);
  T.sell("d", 10);
  //T.showTree();
  T.sell("a", 4);
  //T.showTree();
  T.sell("bread", 7);
  //T.showTree();
  T.sell("mushrooms", 5);
  //T.showTree();
  //std::cout << "rank of 'a' is " << T.rank("a") << std::endl;
}
void test7(size_t data)
{
  std::cout << "TEST 7 -> random selling" << std::endl;
  Bestsellers<std::string> T;
  size_t maximum = 0;
  size_t amount;
  for (size_t i = 0; i < data; i++)
  {
    maximum += amount = rand() % (data * 2);
    T.sell("a" + std::to_string(i), amount);
    //T.showTree();
  }
  //T.showTree();
  amount = 1;
  for (size_t i = 0; i < data; i++)
  {
    maximum += amount;
    //T.showTree();
    T.sell("a" + std::to_string(i), amount);
  }
  //T.showTree();
  for (size_t i = 1; i <= data; i++)
  {
    // std::cout << "test nr. " << i << std:: endl;
    assert(T.rank(T.product(i)) == i);
  }
  assert(T.sold(1, data) == maximum);
}
void test75(size_t data, size_t tests)
{
  std::cout << "TEST 7 -> random selling" << std::endl;
  Bestsellers<std::string> T;
  size_t maximum = 0;
  size_t amount;
  for (size_t i = 0; i < data; i++)
  {
    maximum += amount = rand() % (data * 2);
    T.sell("a" + std::to_string(i), amount);
    //T.showTree();
  }
  //T.showTree();
  amount = 1;
  for (size_t i = 0; i < tests; i++)
  {
    maximum += amount;
    //T.showTree();
    T.sell("a" + std::to_string(rand() % data), amount);
  }
  //T.showTree();
  for (size_t i = 1; i <= data; i++)
  {
    // std::cout << "test nr. " << i << std:: endl;
    assert(T.rank(T.product(i)) == i);
  }
  assert(T.sold(1, data) == maximum);
}
void test8()
{
  std::cout << "TEST 0 -> edge case1" << std::endl;
  Bestsellers<std::string> T;
  T.sell("coke", 12);
  //T.showTree();
  T.sell("bread", 6);
  //T.showTree();
  T.sell("coke", 1);
  //T.showTree();
  T.sell("bread", 1);
  //T.showTree();
  T.sell("ham", 10);
  //T.showTree();
  T.sell("ham", 32);
  //T.showTree();
  T.sell("honey", 3);
  //T.showTree();
  T.sell("money", 8);
  //T.showTree();
  T.sell("bread", 1);
  //T.showTree();
  T.sell("honey", 60);
  //T.showTree();
}
void test9()
{
  std::cout << "TEST 9 -> edge case2" << std::endl;
  Bestsellers<std::string> T;
  T.sell("coke", 12);
  //T.showTree();
  T.sell("coke", 12);
  //T.showTree();
}

int main()
{
  srand(time(0));
  test0();
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test7(30);
  test75(15, 10000);
  test75(100, 100000);
  test7(3000);
  test7(10000);
  test7(50000);
  test7(100000);
  test8();
  test9();
  std::cout << "FINISHED" << std::endl;
}

#endif