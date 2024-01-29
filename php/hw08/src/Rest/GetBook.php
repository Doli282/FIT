<?php declare(strict_types=1);

namespace Books\Rest;

use Books\Database\DB;
use Slim\Psr7\Request;
use Slim\Psr7\Response;

class GetBook
{
    public function getBook (Request $request, Response $response, $args): Response {

        $id = $args['id'];
        if(($id = filter_var($id, FILTER_VALIDATE_INT)) === false)
        {
            $response = new Response();
            return $response->withStatus(400);
        }

        $db = DB::get();
        $query = 'SELECT * FROM `books` WHERE `id` = :id;';
        $statement = $db->prepare($query);
        $statement->execute(['id' => $id]);
        $data = $statement->fetch();

        if(empty($data))
        {
            $response = new Response();
            return $response->withStatus(404);
        }

        $payload = json_encode($data);
        $response->getBody()->write($payload);
        return $response->withHeader('Content-Type', 'application/json')->withStatus(200);
    }
}