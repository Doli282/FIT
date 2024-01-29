<?php declare(strict_types=1);

namespace public;

require_once __DIR__ .'/../MessageTrait.php';

use App\Db;
use App\Model\Account;
use App\Model\Transaction;
use MessageTrait;
use PHPUnit\Framework\TestCase;

class AInitDatabaseTest extends TestCase
{
    use MessageTrait;

    public function setUp(): void
    {
        Transaction::dropTable();
        Account::dropTable();
        Account::createTable();
        Transaction::createTable();

        print 'test';
    }

    public function testInitSelectInDatabase(): void
    {
        print "Public tests running.... \n";

        $queryAccount = "INSERT INTO accounts (number, code) VALUES (:number, :code)";
        $data = ['number' => '330330', 'code' => '0800'];
        $data2 = ['number' => '330330', 'code' => '0801'];
        $accountStatement = Db::get()->prepare($queryAccount);
        $this->assertTrue($accountStatement->execute($data), self::RESULT_CHAR_FAILED .'Test inserting account data 1 failed');
        $this->assertTrue($accountStatement->execute($data2), self::RESULT_CHAR_FAILED .'Test inserting account data 2 failed');

        $query = "INSERT INTO transactions (account_from, account_to, amount) VALUES (:account_from, :account_to, :amount)";
        $data = ['account_from' => 1, 'account_to' => 2, 'amount' => 4444];
        $statement = Db::get()->prepare($query);
        $this->assertTrue($statement->execute($data), self::RESULT_CHAR_FAILED .'Test inserting transaction data failed');

        $query = "SELECT * FROM accounts;";
        $statement = Db::get()->prepare($query);
        $this->assertTrue($statement->execute(), self::RESULT_CHAR_FAILED .'Test account selection failed');
        $result = $statement->fetchAll();
        $this->assertCount(2, $result, self::RESULT_CHAR_FAILED .'Test account selection failed');

        print self::RESULT_CHAR_SUCCESS . "Init DB tests successfully passed!\n\n";
    }
}
