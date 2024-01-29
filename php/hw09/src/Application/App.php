<?php declare(strict_types=1);

namespace BIPHP\Application;

use BIPHP\Entity\Product;
use BIPHP\Entity\ProductResult;

use BIPHP\Entity\RatingResult;
use Symfony\Component\BrowserKit\HttpBrowser;
use Symfony\Component\DomCrawler\Crawler;


/**
 * This class represents your scrapping application
 */
class App
{
    /** @var ProductResult[] */
    private array $productResults = [];




    public function run(): void
    {
        $this->productResults = $this->getProducts("phone", [1, 3], 25000);
        usort($this->productResults, fn(ProductResult $a, ProductResult $b) => $a->getTotalPrice() - $b->getTotalPrice());
    }


    /**
     * Method which returns all products which we search. Products are ordered by total price.
     * @return ProductResult[]
     */
    public function getProductResults(): array
    {
        return $this->productResults;
    }

    /**
     * Method which returns averages for all products we have found form web.
     * @return array<int, float> - it is an array of averages. Product id is the key.
     * @example ['1' => 55.4, '2' => 67.8, '3' => 79,3 ... ]
     */
    public function getProductsRatingAvg(): array
    {
        $result = [];
        $allProducts = $this->getProducts();
        foreach($allProducts as $product)
        {
            $id = $product->getId();
            $sum = 0;
            $count = 0;
            foreach ($product->getRatings() as $rating)
            {

                $sum += $rating->getRating();
                $count++;
            }
            if($count > 0)
            {
                $result[$id] = $sum/$count;
            }
        }
        return $result;
    }

    private function getProducts(string $searched = "", array $excludedIDs = [], int $upperLimit = 0) : array
    {
        $currentProductResults = [];
        // find every product
        $client = new HttpBrowser();
        $url = 'https://bi-php.urbanec.cz/products';
        if(!empty($searched))
        {
            $url = $url.'/search?form%5Bsearch%5D='.$searched;
        }

        $crawler = $client->request('GET', $url);
        $crawler->filter('.card-body')->each(function (Crawler $node) use ($excludedIDs, $upperLimit, &$currentProductResults)
        {
            // move to the Product details
            $link = $node->selectLink('Buy')->link();
            $uri = $link->getUri();

            // get the id from the URI
            $id = filter_var(str_replace('https://bi-php.urbanec.cz/product-detail/', '', $uri), FILTER_VALIDATE_INT);

            // get rid of impostors in the search (they are not phones)
            if(in_array($id, $excludedIDs, false))
            {
                return;
            }

            // search through the details
            $client2 = new HttpBrowser();
            $crawler2 = $client2->request('GET', $uri);

            // create the new Product
            $product = new ProductResult([], $id);

            // filter and assign Product attributes
            $totalPrice = filter_var($crawler2->filter('#total_price')->text(),FILTER_SANITIZE_NUMBER_FLOAT, FILTER_FLAG_ALLOW_FRACTION);
            // try to find discount if there is any. If not, use the default price
            try{
                $discountPrice = filter_var($crawler2->filter('#total_price_discount')->text(),FILTER_SANITIZE_NUMBER_FLOAT, FILTER_FLAG_ALLOW_FRACTION);
                if(($upperLimit > 0) && ($discountPrice > $upperLimit))
                {
                    return;
                }
                $product->setTotalPrice((float)$discountPrice);
                $product->setTotalPriceWithoutDiscount((float)$totalPrice);
            }
            catch (\Exception $exception)
            {
                if(($upperLimit > 0) && ($totalPrice > $upperLimit))
                {
                    return;
                }
                $product->setTotalPrice((float)$totalPrice);
            }
            $name = $crawler2->filter('#name')->text();
            $desc = $crawler2->filter('#desc')->text();

            $product->setName($name);
            $product->setDescription($desc);
            $product->setLink($uri);

            // find ratings
            $crawler2->filter('div.card.w-100.mb-3.border-warning-subtle.shadow > .card-body > .card-body')->each(function (Crawler $node) use($product){
                $username = $node->filter('.card-title')->text();
                $comment = $node->filter('.card-text')->text();
                $rating = filter_var($node->filter('.progress-bar')->text(), FILTER_VALIDATE_INT);
                $review = new RatingResult($username, $comment, $rating);
                $product->addRating($review);
            });

            // add to the list
            $currentProductResults[] = $product;
        });
        return $currentProductResults;
    }
}
