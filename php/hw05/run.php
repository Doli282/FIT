<?php declare(strict_types=1);

use App\Builder;
use App\Renderer;

/*
 * usage:
 *
 * php run.php > invoice.pdf
 *
 */

// TODO use composer autoloader - done
require __DIR__ . '/vendor/autoload.php';

$builder = new Builder();
$invoice = $builder
    ->setNumber('201900021')
    ->setSupplier(
        'Dodavatel Holding a.s.', 'CZ66776677', 'Kratka', "2", 'Brno', '60200', '+420 999 100 999', 'info@d-holding.eu'
    )
    ->setCustomer('Jan Novak', 'CZ12345678', 'Dlouha', "1", 'Praha', '11000', null, 'cutomer@testing.cz')
    ->addItem('Zbozi', 15.5, 199.99)
    ->addItem('Sluzby', 1, 98100.57)
    ->build();

$builder = new Builder();
$invoice2 = $builder
    ->setNumber('123456789123456789123456789123456789123456789123456789987654321000')
    ->setSupplier(
        'Dodavatel Holding a.s.', 'CZ66776677', 'Kratka', "2", 'Brno', '60200', '+420 999 100 999', 'info@d-holding.eu'
    )
    ->setCustomer('Jan Novak', 'CZ12345678', 'Dlouha', "1", 'Praha', '11000', null, 'cutomer@testing.cz')
    ->addItem('Zbozi', 15.5, 199.99)
    ->addItem('Sluzby', 1, 98100.57)
    ->build();

$builder = new Builder();
$invoice3 = $builder
    ->setNumber('0001')
    ->setSupplier(
        'Dodavatel Holding a.s. with help of company and others including Adam, Beata, Ciri, Daniel, Eve, Filip, Giuseppe, Hans, Ivan, Jaroslav, Lukas, and etc.', 'CZ6677667700000123456789123', 'Kratka ale vlastne ne zas tak kratka, mona i dost dlouha', "2", 'Brno - prazske to predmesti', '6', '+420 999 100 999', 'info@d-holding.eu'
    )
    ->setCustomer('Jan Novak', 'CZ12345678', 'Dlouha', "1", 'Praha', '11000', '+420123456789', null)
    ->addItem('Zbozi', 15.5, 199.99)
    ->addItem('Sluzby', 1, 98100.57)
    ->addItem('Sluzby1', 100000000, 98100.57)
    ->addItem('Sluzby2', 0, 98100.57)
    ->addItem('Sluzby3', 1.5, 98100.57)
    ->addItem('Sluzby4', 1.66666666666, 98100.57)
    ->addItem('Sluzby7', 1, 98100987654321321.57);
for($i = 0; $i < 30; $i++)
    {
        $builder->addItem('product' . $i, $i, $i);
    }
$invoice3 = $builder->build();

echo (new Renderer)->makePdf($invoice);
