<?php declare(strict_types=1);

namespace Books\Middleware;

use Psr\Http\Message\ResponseInterface as Response;
use Psr\Http\Message\ServerRequestInterface as Request;
use Psr\Http\Server\MiddlewareInterface;
use Psr\Http\Server\RequestHandlerInterface as RequestHandler;

class HTTPBasicAuthMiddleware  implements MiddlewareInterface
{

    private string $username = 'admin';
    private string $password = 'pas$word';

    public function process(Request $request, RequestHandler $handler): Response
    {
        if(!$request->hasHeader('Authorization'))
        {
            $response = new \Slim\Psr7\Response();
            return $response->withStatus(401);
        }
        $authHeader = $request->getHeaderLine('Authorization');
        // basic token
        $token = str_replace('Basic ', '', $authHeader);
        // username:pass
        $authParams = explode(':', base64_decode($token));
        $login = $authParams[0];
        $pass = $authParams[1];

        if(($login === $this->username) && ($pass === $this->password))
        {
            return $handler->handle($request);
        }
        else
        {
            $response = new \Slim\Psr7\Response();
            return $response->withStatus(401);
        }
    }
}