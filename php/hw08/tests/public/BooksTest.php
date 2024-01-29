<?php

namespace public;

use Books\Rest\RestApp;
use HelperFactory;
use Fig\Http\Message\StatusCodeInterface;
use MessageTrait;
use PHPUnit\Framework\TestCase;
use Slim\App;

class BooksTest extends TestCase
{
    use MessageTrait;

    private App $app;

    public function __construct(?string $name = null, array $data = [], $dataName = '')
    {
        parent::__construct($name, $data, $dataName);
        $restApp = new RestApp();
        $restApp->configure();
        $this->app = $restApp->getApp();
    }

    public function setUp(): void
    {
        HelperFactory::createDB();
        HelperFactory::insertBooks();
    }

    public function tearDown(): void
    {
        HelperFactory::dropDB();
    }

    public function testGetBooks(): void
    {
        print "Public test running...\n\n";
        $request = HelperFactory::createRequest('GET', '/books');
        $response = $this->app->handle($request);
        $this->assertEquals(StatusCodeInterface::STATUS_OK, $response->getStatusCode(), self::RESULT_CHAR_FAILED .'Test GET books failed. Invalid status code.');
        $body = $response->getBody();
        $books = json_decode($body, true);
        $this->assertNotNull($books);
        $this->assertIsArray($books);
        $this->assertCount(3, $books);
        foreach ($books as $book) {
            $this->assertArrayHasKey('id', $book, self::RESULT_CHAR_FAILED .'Test GET books failed. Invalid json.');
            $this->assertArrayHasKey('name', $book, self::RESULT_CHAR_FAILED .'Test GET books failed. Invalid json.');
            $this->assertArrayHasKey('author', $book, self::RESULT_CHAR_FAILED .'Test GET books failed. Invalid json.');
            $this->assertArrayNotHasKey('isbn', $book, self::RESULT_CHAR_FAILED .'Test GET books failed. Invalid json.');
            $this->assertArrayNotHasKey('publisher', $book, self::RESULT_CHAR_FAILED .'Test GET books failed. Invalid json.');
        }
        print self::RESULT_CHAR_SUCCESS . "GET /books tests successfully passed!\n\n";
    }

    public function testGetBookDetail(): void
    {
        $request = HelperFactory::createRequest('GET', '/books/5');
        $response = $this->app->handle($request);
        $this->assertEquals(StatusCodeInterface::STATUS_OK, $response->getStatusCode(), self::RESULT_CHAR_FAILED .'Test GET books/:id failed. Invalid response code.');
        $body = $response->getBody();
        $book = json_decode($body, true);
        $this->assertNotNull($book);
        $this->assertIsArray($book);
        $this->assertArrayHasKey('id', $book, self::RESULT_CHAR_FAILED .'Test GET books/:id failed. Invalid json.');
        $this->assertArrayHasKey('name', $book, self::RESULT_CHAR_FAILED .'Test GET books/:id failed. Invalid json.');
        $this->assertArrayHasKey('author', $book, self::RESULT_CHAR_FAILED .'Test GET books/:id failed. Invalid json.');
        $this->assertArrayHasKey('isbn', $book, self::RESULT_CHAR_FAILED .'Test GET books/:id failed. Invalid json.');
        $this->assertArrayHasKey('publisher', $book, self::RESULT_CHAR_FAILED .'Test GET books/:id failed. Invalid json.');

        $this->assertEquals(5, $book['id'], self::RESULT_CHAR_FAILED .'Test GET books/:id failed. Invalid book.');
        $this->assertEquals('Svátý Chlast', $book['name'], self::RESULT_CHAR_FAILED .'Test GET books/:id failed. Invalid book.');
        $this->assertEquals('1-84356-028-1', $book['isbn'], self::RESULT_CHAR_FAILED .'Test GET books/:id failed. Invalid book.');
        print self::RESULT_CHAR_SUCCESS . "GET /books/:id tests successfully passed!\n\n";
    }

    public function testPostBook(): void
    {
        $newBook = [
            'name' => "BI-PHP Proč studenti rádí dělají úkoly?",
            'author' => "Andrii Plyskach",
            'publisher' => "Vydavatelství svátý oříšek",
            'isbn' => "1-84356-888-3",
            'pages' => 188
        ];
        $headers = [
            'HTTP_ACCEPT' => 'application/json',
            'Authorization' => $this->getAuth()
        ];
        $request = HelperFactory::createRequest(
            method: 'POST',
            path:'/books',
            headers: $headers,
        );
        $request = $request->withParsedBody($newBook);
        $response = $this->app->handle($request);
        $this->assertEquals(StatusCodeInterface::STATUS_CREATED, $response->getStatusCode(), self::RESULT_CHAR_FAILED .'Test POST /books failed. Invalid status code.');
        $request = HelperFactory::createRequest('GET', '/books/7');
        $response = $this->app->handle($request);
        $this->assertEquals(StatusCodeInterface::STATUS_OK, $response->getStatusCode(), self::RESULT_CHAR_FAILED .'Test POST /books failed. Invalid status code.');

        $request = HelperFactory::createRequest(method: 'POST', path:'/books');
        $request = $request->withParsedBody($newBook);
        $result = $this->app->handle($request);
        $this->assertEquals(StatusCodeInterface::STATUS_UNAUTHORIZED, $result->getStatusCode(), self::RESULT_CHAR_FAILED .'Test POST /books failed. Invalid status code.');
        print self::RESULT_CHAR_SUCCESS . "POST /books tests successfully passed!\n\n";
    }

    public function testPutBook(): void
    {
        $newBook = [
            'name' => "Sestavení rozvrhu není jednoduché",
            'author' => "Daniel Domek",
            'publisher' => "Vydavatelství svátý oříšek",
            'isbn' => "1-7777-044-3",
            'pages' => 224
        ];
        $headers = ['HTTP_ACCEPT' => 'application/json', 'Authorization' => $this->getAuth()];
        $request = HelperFactory::createRequest(
            method: 'PUT',
            path:'/books/4',
            headers: $headers,
        );
        $request = $request->withParsedBody($newBook);
        $response = $this->app->handle($request);
        $this->assertEquals(StatusCodeInterface::STATUS_NO_CONTENT, $response->getStatusCode(), self::RESULT_CHAR_FAILED .'Test PUT books/:id failed');
        $request = HelperFactory::createRequest(method: 'PUT', path:'/books/4');
        $request = $request->withParsedBody($newBook);
        $result = $this->app->handle($request);
        $this->assertEquals(StatusCodeInterface::STATUS_UNAUTHORIZED, $result->getStatusCode(), self::RESULT_CHAR_FAILED .'Test PUT books/:id failed. Invalid status code.');
        print self::RESULT_CHAR_SUCCESS . "PUT /books/:id tests successfully passed!\n\n";
    }

    public function testDeleteBook(): void
    {
        $headers = ['HTTP_ACCEPT' => 'application/json', 'Authorization' => $this->getAuth()];
        $request = HelperFactory::createRequest(
            method: 'DELETE',
            path:'/books/4',
            headers: $headers
        );
        $response = $this->app->handle($request);
        $this->assertEquals(StatusCodeInterface::STATUS_NO_CONTENT, $response->getStatusCode(), self::RESULT_CHAR_FAILED .'Test DELETE books/:id failed');
        $request = HelperFactory::createRequest(
            method: 'GET',
            path:'/books/4',
        );
        $result = $this->app->handle($request);
        $this->assertEquals(StatusCodeInterface::STATUS_NOT_FOUND, $result->getStatusCode(), self::RESULT_CHAR_FAILED .'Test DELETE books/:id failed. Invalid status code.');
        print self::RESULT_CHAR_SUCCESS . "DELETE /books/:id tests successfully passed!\n\n";
    }

    private function getAuth(): string
    {
        return 'Basic ' . base64_encode('admin:pas$word');
    }
}
