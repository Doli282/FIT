<?php

namespace Public;

use BIPHP\Application\App;
use BIPHP\Entity\ProductResult;
use PHPUnit\Framework\TestCase;

class PublicTest extends TestCase
{
    private static ?App $app = null;

    public static function setUpBeforeClass(): void
    {
        sleep(1);
        self::$app = new App();
        self::$app->run();
    }

    public function testProducts(): void
    {
        $app = self::$app;
        $this->assertNotNull($app);
        $products = $app->getProductResults();
        $this->assertIsArray($products);
        $this->assertNotEmpty($products);

        // get specific product
        /** @var ProductResult $product */
        $product = current(array_filter($products, fn(ProductResult $p) => $p->getId() === 15));
        $this->assertNotNull($product);
        $this->assertEquals(15, $product->getId(), 'Invalid product id');
        $this->assertEquals('Samsung Galaxy S8+', $product->getName(), 'Invalid product name');
        $this->assertEquals(9438, $product->getTotalPrice(), 'Invalid total price');
        $this->assertNull($product->getTotalPriceWithoutDiscount(), '');

        // get specific product
        /** @var ProductResult $product */
        $product = current(array_filter($products, fn(ProductResult $p) => $p->getId() === 7));
        $this->assertFalse($product, 'Invalid Product');
        $product = current(array_filter($products, fn(ProductResult $p) => $p->getId() === 43));
        $this->assertFalse($product, 'Invalid product');
    }


    public function testPrices(): void
    {
        $app = self::$app;
        $this->assertNotNull($app);
        $products = $app->getProductResults();
        $results = array_filter($products, fn (ProductResult $p) => $p->getTotalPrice() > 25000);
        $this->assertEmpty($results);

        $first = current($products);
        $last = end($products);

        $this->assertNotEquals(false, $first);
        $this->assertNotEquals(false, $last);

        $this->assertGreaterThan($first->getTotalPrice(), $last->getTotalPrice(), 'Invalid product ordering');
    }
}
