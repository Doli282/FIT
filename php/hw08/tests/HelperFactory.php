<?php

use Books\Database\DB;
use Slim\Psr7\Factory\StreamFactory;
use Slim\Psr7\Headers;
use Slim\Psr7\Request;
use Slim\Psr7\Uri;

class HelperFactory
{
    public static function createDB(): void
    {
        $pdo = DB::get();
        $query = <<<SQL
            CREATE TABLE IF NOT EXISTS books (
                id INTEGER PRIMARY KEY,
                name TEXT NOT NULL,
                author TEXT NOT NULL,
                publisher TEXT NOT NULL,
                isbn TEXT NOT NULL UNIQUE,
                pages INTEGER NOT NULL
            );
        SQL;
        $st = $pdo->query($query);
        $st->execute();
    }

    public static function dropDB(): void
    {
        $pdo = DB::get();
        $st = $pdo->query("DELETE FROM books");
        $st->execute();
    }

    public static function insertBooks(): void
    {
        $book1 = [
            'id' => 4,
            'name' => "Sestavení rozvrhu není jednoduché",
            'author' => "Daniel Domek",
            'publisher' => "Vydavatelství svátý oříšek",
            'isbn' => "1-84356-044-3",
            'pages' => 224
        ];
        $book2 = [
            'id' => 5,
            'name' => "Svátý Chlast",
            'author' => "Michal Chlast",
            'publisher' => "Vydavatelství svátý oříšek",
            'isbn' => "1-84356-028-1",
            'pages' => 88
        ];
        $book3 = [
            'id' => 6,
            'name' => "Jak vyhodit studenta z vysoké školy",
            'author' => "Ing. Ladislav Vagner",
            'publisher' => "Vydavatelství svátý oříšek",
            'isbn' => "1-84356-028-3",
            'pages' => 784
        ];
        $pdo = DB::get();
        $query = "Insert into books (id, name, author, publisher, isbn, pages) 
            VALUES (:id, :name, :author, :publisher, :isbn, :pages);";
        $st = $pdo->prepare($query);
        $st->execute($book1);
        $st->execute($book2);
        $st->execute($book3);
    }

    public static function createRequest(
        string $method,
        string $path,
        array $headers = ['HTTP_ACCEPT' => 'application/json'],
        array $cookies = [],
        array $serverParams = [],
    ): Request {
        $uri = new Uri('', '', 80, $path);
        $handle = fopen('php://temp', 'w+');
        $stream = (new StreamFactory())->createStreamFromResource($handle);
        $h = new Headers();
        foreach ($headers as $name => $value) {
            $h->addHeader($name, $value);
        }

        return new Request($method, $uri, $h, $cookies, $serverParams, $stream);
    }
}
