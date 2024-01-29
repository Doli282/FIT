<?php declare(strict_types=1);

namespace Books\Rest;

use Books\Database\DB;
use Slim\Psr7\Request;
use Slim\Psr7\Response;

class DeleteBook
{
    public static function deleteBook(Request $request, Response $response, $args): Response
    {
        $id = $args['id'];
        if(($id = filter_var($id, FILTER_VALIDATE_INT)) === false)
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

        $query = 'DELETE FROM `books` WHERE `id` = :id;';
        $statement = $db->prepare($query);
        $statement->execute(['id' => $id]);

        return $response->withStatus(204);
    }
}