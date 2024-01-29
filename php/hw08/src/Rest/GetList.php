<?php declare(strict_types=1);

namespace Books\Rest;

use Books\Database\DB;
use Slim\Psr7\Request;
use Slim\Psr7\Response;

class GetList
{
    public function getList (Request $request, Response $response): Response {

        $db = DB::get();
        $query = 'SELECT `id`, `name`, `author` FROM `books`';
        $data = $db->query($query);
        $payload = json_encode($data->fetchAll());
        $response->getBody()->write($payload);
        $response->withHeader('Content-Type', 'application/json');
        $response->withStatus(200);
        return $response;
    }
}