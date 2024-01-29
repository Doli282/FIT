<?php declare(strict_types=1);

namespace Books\Rest;

use Books\Database\DB;
use Slim\Psr7\Request;
use Slim\Psr7\Response;

class PostBook
{
    public static function createBook (Request $request, Response $response): Response
    {
        try {
            $params = (array)$request->getParsedBody();
            $name = $params['name'];
            $author = $params['author'];
            $publisher = $params['publisher'];
            $isbn = $params['isbn'];
            $pages = $params['pages'];
        }
        catch (\Exception $ex)
        {
            $response = new Response();
            return $response->withStatus(400);
        }

        if(empty($name) || empty($author) || empty($publisher) || empty($isbn) || empty($pages))
        {
            $response = new Response();
            return $response->withStatus(400);
        }

        $db = DB::get();
        $query = 'INSERT INTO `books` (`name`, `author`, `publisher`, `isbn`, `pages`) VALUES (:name, :author, :publisher, :isbn, :pages);';
        $statement = $db->prepare($query);
        $statement->execute(['name' => $name, 'author' => $author, 'publisher' => $publisher, 'isbn' => $isbn, 'pages' => $pages]);
        $id = $db->lastInsertId();

        $response = $response->withHeader('Location', '/books/:'.$id)->withStatus(201);;
        return $response;
    }
}