<?php declare(strict_types=1);

namespace App;

use Dompdf\Dompdf;
use Latte\Engine;


class Renderer extends Dompdf
{
    public function makeHtml(Invoice $invoice): string
    {
        // TODO implement - done
        $latte = new Engine();
        $template = __DIR__.DIRECTORY_SEPARATOR.'invoice.latte.html';
        return $latte->renderToString($template, ['invoice' => $invoice]);
    }
    public function makePdf(Invoice $invoice): string
    {
        // TODO implement - done
        $this->loadHtml($this->makeHtml($invoice));
        $this->setPaper('A4', 'portrait');
        $this->render();
        return $this->output();
    }
}
