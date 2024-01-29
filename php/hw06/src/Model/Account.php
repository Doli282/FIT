<?php declare(strict_types=1);

namespace App\Model;

use App\Db;
use PDO;

class Account
{
    public function __construct(
        protected int    $id,
        protected string $number,
        protected string $code
    )
    {
    }

    /**
     * Creates DB table using CREATE TABLE ...
     */
    public static function createTable(): void
    {
        $db = Db::get();
        $db->query('CREATE TABLE IF NOT EXISTS `accounts` (
            -- TODO implement - done
            `id` INTEGER PRIMARY KEY,
            `number` TEXT,
            `code` TEXT,
            UNIQUE(number, code)
        )');
    }

    /**
     * Drops DB table using DROP TABLE ...
     */
    public static function dropTable(): void
    {
        // TODO implement - done
        $db = Db::get();
        $db->query('PRAGMA foreign_keys = OFF;');
        if(!$db->query('DROP TABLE IF EXISTS `accounts`;'))
        {
            echo "dropped";
            $db->query('UPDATE `transactions`;');
            $db->query('SET `account_from` = NULL;');
            $db->query('SET `account_to` = NULL;');
        }
        $db->query('PRAGMA foreign_keys = ON;');
    }

    /**
     * Find account record by number and bank code
     */
    public static function find(string $number, string $code): ?self
    {
        // TODO implement - done
        $db = Db::get();
        $query = 'SELECT * FROM `accounts` WHERE number = :number AND code = :code;';
        $statement = $db->prepare($query);
        $statement->execute(['number' => $number, 'code' => $code]);
        $result = $statement->fetch();
        if($result === false)
        {
            return null;
        }
        else
        {
            return (new Account($result['id'], $result['number'], $result['code']));//->setCode($result['code'])->setNumber($result['code']);
        }
    }

    /**
     * Find account record by id
     */
    public static function findById(int $id): ?self
    {
        // TODO implement - done
        $db = Db::get();
        $query = 'SELECT * FROM `accounts` WHERE id = :id;';
        $statement = $db->prepare($query);
        $statement->execute(['id' => $id]);
        $result = $statement->fetch(PDO::FETCH_NAMED);
        if($result === false)
        {
            return null;
        }
        else
        {
            return (new Account($result['id'], $result['number'], $result['code']));
        }
    }

    /**
     * Inserts new account record and returns its instance; or returns existing account instance
     */
    public static function findOrCreate(string $number, string $code): self
    {
        // TODO implement - done
        $db = Db::get();
        $queryI = 'INSERT OR IGNORE INTO `accounts` (`number`, `code`) VALUES( :number , :code);';
        $statement = $db->prepare($queryI);
        $statement->execute(['number' => $number, 'code' => $code]);
        $queryS = 'SELECT * FROM `accounts` WHERE `number` = :number AND `code` = :code;';
        $statement = $db->prepare($queryS);
        $statement->execute(['number' => $number, 'code' => $code]);
        $result = $statement->fetch(PDO::FETCH_NAMED);
        if($result === false)
        {
            echo "unable to FindOrCreate - error".PHP_EOL;
            return new Account(0,"0", "0");
        }

        return (new Account($result['id'], $number, $code));
    }

    /**
     * Returns iterable of Transaction instances related to this Account, consider both transaction direction
     *
     * @return iterable<Transaction>
     */
    public function getTransactions(): iterable
    {
        // TODO implement - done
        $result = array();
        $db = Db::get();
        // query for transactions
        $queryT = 'SELECT * FROM `transactions` WHERE `account_from` = :id OR `account_to` = :id;';
        $statementT = $db->prepare($queryT);
        $statementT->execute(['id' => $this->getId()]);

        // query for accounts
        $queryA = 'SELECT * FROM `accounts` WHERE `id` = :id;';
        $statementA = $db->prepare($queryA);

        // for every incoming transaction ...
        while ($row = $statementT->fetch(PDO::FETCH_NAMED, PDO::FETCH_ORI_NEXT)) {

            if($row['account_from'] == $this->id){
                $account_from = $this;
            }
            else
            {
                $statementA->execute(['id' => $row['account_from']]);
                $account = $statementA->fetch(PDO::FETCH_ASSOC);
                $account_from = new Account($account['id'], $account['number'], $account['code']);
            }

            if($row['account_to'] == $this->id){
                $account_to = $this;
            }
            else
            {
                $statementA->execute(['id' => $row['account_to']]);
                $account = $statementA->fetch(PDO::FETCH_ASSOC);
                $account_to = new Account($account['id'], $account['number'], $account['code']);
            }

            $result[] = new Transaction($account_from, $account_to, $row['amount']);
        }

        return $result;
    }

    /**
     * Returns transaction sum (using SQL aggregate function). Treat outgoing transactions as 'minus' and incoming as 'plus'.
     */
    public function getTransactionSum(): float
    {
        // TODO implement - done
        $db = Db::get();
        $query = 'SELECT
                  SUM(CASE WHEN `account_to` = :id THEN `amount` ELSE 0 END) -
                  SUM(CASE WHEN `account_from` = :id THEN `amount` ELSE 0 END) as `balance`
                  FROM `transactions`
                  WHERE `account_from` = :id OR `account_to` = :id;';
        $statement = $db->prepare($query);
        $statement->execute(['id' => $this->getId()]);
        return ($statement->fetch(PDO::FETCH_NAMED))['balance'];
    }

    public function getId(): int
    {
        return $this->id;
    }

    public function setId(int $id): Account
    {
        $this->id = $id;
        return $this;
    }

    public function getNumber(): string
    {
        return $this->number;
    }

    public function setNumber(string $number): Account
    {
        $this->number = $number;
        return $this;
    }

    public function getCode(): string
    {
        return $this->code;
    }

    public function setCode(string $code): Account
    {
        $this->code = $code;
        return $this;
    }

    public function __toString(): string
    {
        return "{$this->number}/{$this->code}";
    }
}
