<?php declare(strict_types=1);

namespace Books\Rest;

use Books\Middleware\HTTPBasicAuthMiddleware;
use Books\Middleware\JsonBodyParserMiddleware;
use Slim\App;
use Slim\Factory\AppFactory;
use Slim\Psr7\Request;
use Slim\Psr7\Response;

class RestApp
{
    private ?App $app = null;

    public function configure(): void
    {
        $this->app = AppFactory::create();

        $this->app->addRoutingMiddleware();
        $this->app->addErrorMiddleware(true, true, true);
        $this->app->add(new JsonBodyParserMiddleware());

        $this->app->get('/', function (Request $request, Response $response) {
            $response->getBody()->write('Funguje to! Ale nic tady není.');
            return $response;
        });

        // todo implement me please !!!

        $this->app->get('/books', [new GetList(), 'getList']);
        $this->app->get('/books/{id}', [new GetBook(), 'getBook']);
        $this->app->post('/books', [new PostBook(), 'createBook'])->add(new HTTPBasicAuthMiddleware());
        $this->app->put('/books/{id}', [new PutBook(), 'updateBook'])->add(new HTTPBasicAuthMiddleware());
        $this->app->delete('/books/{id}', [new DeleteBook(), 'deleteBook'])->add(new HTTPBasicAuthMiddleware());
    }

    public function run(): void {
        $this->app->run();
    }

    public function getApp(): App {
        return $this->app;
    }
}
