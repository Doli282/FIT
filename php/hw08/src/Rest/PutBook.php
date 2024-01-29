<?php declare(strict_types=1);

namespace Books\Rest;

use Books\Database\DB;
use Slim\Psr7\Request;
use Slim\Psr7\Response;

class PutBook
{
    public static function updateBook(Request $request, Response $response, $args) : Response
    {
        $id = $args['id'];
        if(($id = filter_var($id, FILTER_VALIDATE_INT)) === false)
        {
            $response = new Response();
            return $response->withStatus(400);
        }
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
        $queryCheck = 'SELECT 1 FROM `books` WHERE `id` = :id;';
        $statementCheck = $db->prepare($queryCheck);
        $statementCheck->execute(['id' => $id]);
        $resultCheck = $statementCheck->fetch();
        if(empty($resultCheck))
        {
            $response = new Response();
            return $response->withStatus(404);
        }

        $query = 'UPDATE `books` SET
                   `name` = :name,
                   `author` = :author,
                   `publisher` = :publisher,
                   `isbn` = :isbn,
                   `pages` = :pages
                   WHERE `id` = :id;';
        $statement = $db->prepare($query);
        $statement->execute(['id' => $id, 'name' => $name, 'author' => $author, 'publisher' => $publisher, 'isbn' => $isbn, 'pages' => $pages]);

        return $response->withStatus(204);
    }
}