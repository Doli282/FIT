<?php declare(strict_types=1);

namespace public;

require_once __DIR__ .'/../MessageTrait.php';

use App\Db;
use App\Model\Account;
use App\Model\Transaction;
use MessageTrait;
use PHPUnit\Framework\TestCase;

class TransactionTest extends TestCase
{
    use MessageTrait;

    public function setUp(): void
    {
        Transaction::dropTable();
        Account::dropTable();
        Account::createTable();
        Transaction::createTable();
    }

    private function insertData(): void
    {
        $queryAccount = "INSERT INTO accounts (number, code) VALUES (:number, :code)";
        $a1 = ['number' => '330330', 'code' => '0800'];
        $a2 = ['number' => '330331', 'code' => '0800'];
        $a3 = ['number' => '330332', 'code' => '0800'];
        $accountStatement = Db::get()->prepare($queryAccount);
        $this->assertTrue($accountStatement->execute($a1), self::RESULT_CHAR_FAILED .'Test inserting account data 1 failed');
        $this->assertTrue($accountStatement->execute($a2), self::RESULT_CHAR_FAILED .'Test inserting account data 2 failed');
        $this->assertTrue($accountStatement->execute($a3), self::RESULT_CHAR_FAILED .'Test inserting account data 2 failed');

        $query = "INSERT INTO transactions (account_from, account_to, amount) VALUES (:account_from, :account_to, :amount)";
        $t1 = ['account_from' => 1, 'account_to' => 2, 'amount' => 4444];
        $t2 = ['account_from' => 2, 'account_to' => 1, 'amount' => 40];
        $t3 = ['account_from' => 2, 'account_to' => 3, 'amount' => 500];
        $t4 = ['account_from' => 1, 'account_to' => 3, 'amount' => 456];
        $statement = Db::get()->prepare($query);
        $this->assertTrue($statement->execute($t1), self::RESULT_CHAR_FAILED .'Test inserting transaction data failed');
        $this->assertTrue($statement->execute($t2), self::RESULT_CHAR_FAILED .'Test inserting transaction data failed');
        $this->assertTrue($statement->execute($t3), self::RESULT_CHAR_FAILED .'Test inserting transaction data failed');
        $this->assertTrue($statement->execute($t4), self::RESULT_CHAR_FAILED .'Test inserting transaction data failed');
    }

    public function testTransactionsDatabase(): void
    {
        $this->insertData();
        $account2 = Account::findById(2);
        self::assertNotNull($account2, self::RESULT_CHAR_FAILED .'Account with id = 2 does not exist');

        $transactions = $account2->getTransactions();
        $this->assertCount(3, $transactions, self::RESULT_CHAR_FAILED .'Invalid number of transactions for account 3.');
        foreach($transactions as $tr) {
            $this->assertTrue($tr->getFrom()->getId() === 2 || $tr->getTo()->getId() === 2, self::RESULT_CHAR_FAILED .'Invalid transaction in the list.');
        }

        print self::RESULT_CHAR_SUCCESS . "Transaction tests successfully passed!\n\n";
    }

    public function testTransactionsSumDatabase(): void
    {
        $this->insertData();
        $account3 = Account::findById(3);
        self::assertNotNull($account3, self::RESULT_CHAR_FAILED .'Account with id = 3 does not exist');

        $sum = $account3->getTransactionSum();
        $this->assertEqualsWithDelta(956.0, $sum, 0.00001,self::RESULT_CHAR_FAILED .'Invalid number of transactions for account 3.');

        print self::RESULT_CHAR_SUCCESS . "Transaction tests successfully passed!\n\n";
    }

    public function testTransactionsInsertToDatabase(): void
    {
        $this->insertData();
        $account1 = Account::findById(1);
        $account2 = Account::findById(2);
        self::assertNotNull($account1, self::RESULT_CHAR_FAILED .'Account with id = 1 does not exist');
        self::assertNotNull($account2, self::RESULT_CHAR_FAILED .'Account with id = 2 does not exist');
        $sum = $account1->getTransactionSum();
        $this->assertEquals(-4860.0, $sum, self::RESULT_CHAR_FAILED .'Transaction insert/update failed.');
        $newTr = new Transaction($account1, $account2, 504.3);
        $newTr->insert();
        $sumAfterInsert = $account1->getTransactionSum();
        $this->assertEquals(-5364.3, $sumAfterInsert, self::RESULT_CHAR_FAILED .'Transaction insert/update failed.');

        print self::RESULT_CHAR_SUCCESS . "Transaction insert tests successfully passed!\n\n";
    }
}
