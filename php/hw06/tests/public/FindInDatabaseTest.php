<?php declare(strict_types=1);

namespace public;

require_once __DIR__ .'/../MessageTrait.php';
require_once __DIR__ . '/AInitDatabaseTest.php';

use App\Db;
use App\Model\Account;
use App\Model\Transaction;
use MessageTrait;
use PHPUnit\Framework\TestCase;

class FindInDatabaseTest extends TestCase
{
    use MessageTrait;

    public function setUp(): void
    {
        Transaction::dropTable();
        Account::dropTable();
        Account::createTable();
        Transaction::createTable();
    }


    public function testFindMethod(): void
    {
        $queryAccount = "INSERT INTO accounts (number, code) VALUES (:number, :code)";
        $data = ['number' => '330330', 'code' => '0800'];
        $data2 = ['number' => '330331', 'code' => '0801'];
        $accountStatement = Db::get()->prepare($queryAccount);
        $this->assertTrue($accountStatement->execute($data), self::RESULT_CHAR_FAILED .'Inserting account data 1 failed');
        $this->assertTrue($accountStatement->execute($data2), self::RESULT_CHAR_FAILED .'Inserting account data 2 failed');

        $this->assertNotNull(Account::find('330330','0800'), self::RESULT_CHAR_FAILED .'Find method test failed');
        $this->assertNull(Account::find('330331','0800'), self::RESULT_CHAR_FAILED .'Find method test 2 failed');
        $this->assertNotNull(Account::find('330331','0801'), self::RESULT_CHAR_FAILED .'Find method test 3 failed');
        $this->assertNull(Account::find('',''), self::RESULT_CHAR_FAILED .'Find method test 4 failed');

        $account = Account::findById(1);
        $this->assertNotNull($account, self::RESULT_CHAR_FAILED .'FindById method test 1 failed');
        $this->assertEquals('0800', $account->getCode(), self::RESULT_CHAR_FAILED .'FindById method test 1 failed');
        $this->assertNull(Account::findById(14), self::RESULT_CHAR_FAILED .'FindById method test 2 failed');
        $this->assertNull(Account::findById(0), self::RESULT_CHAR_FAILED .'FindById method test 3 failed');
        $this->assertNotNull(Account::findById(2), self::RESULT_CHAR_FAILED .'FindById method test 4 failed');

        $account = Account::findOrCreate('494949', '2588');
        $this->assertNotNull($account);
        $this->assertEquals('2588', $account->getCode(), self::RESULT_CHAR_FAILED .'FindOrCreate method test 1 failed');
        $this->assertNotNull(Account::find('494949', '2588'), self::RESULT_CHAR_FAILED .'FindOrCreate method test 2 failed');
        $this->assertNotNull(Account::findById(3), self::RESULT_CHAR_FAILED .'FindOrCreate method test 2 failed');
        $account = Account::findOrCreate('494949', '2588');
        $this->assertEquals(3, $account->getId(), self::RESULT_CHAR_FAILED .'FindOrCreate method test 3 failed');

        print self::RESULT_CHAR_SUCCESS . "Find methods tests successfully passed!\n\n";
    }

    public function testFindOrCreateMethod(): void
    {
        $queryAccount = "INSERT INTO accounts (number, code) VALUES (:number, :code)";
        $data = ['number' => '330330', 'code' => '0800'];
        $data2 = ['number' => '330331', 'code' => '0801'];
        $accountStatement = Db::get()->prepare($queryAccount);
        $this->assertTrue($accountStatement->execute($data), self::RESULT_CHAR_FAILED .'Inserting account data 1 failed');
        $this->assertTrue($accountStatement->execute($data2), self::RESULT_CHAR_FAILED .'Inserting account data 2 failed');


        $account = Account::findOrCreate('494949', '2588');
        $this->assertNotNull($account);
        $this->assertEquals('2588', $account->getCode(), self::RESULT_CHAR_FAILED .'FindOrCreate method test 1 failed');
        $this->assertNotNull(Account::find('494949', '2588'), self::RESULT_CHAR_FAILED .'FindOrCreate method test 2 failed');
        $this->assertNotNull(Account::findById(3), self::RESULT_CHAR_FAILED .'FindOrCreate method test 2 failed');
        $account = Account::findOrCreate('494949', '2588');
        $this->assertEquals(3, $account->getId(), self::RESULT_CHAR_FAILED .'FindOrCreate method test 3 failed');

        print self::RESULT_CHAR_SUCCESS . "Find or create method tests successfully passed!\n\n";
    }
}
