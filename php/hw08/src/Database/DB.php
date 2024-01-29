<?php declare(strict_types=1);

namespace Books\Database;

use PDO;

class DB
{
    protected static ?PDO $pdo = null;

    public static function getConnection(): PDO
    {
        return self::get();
    }

    public static function get(): PDO
    {
        /** For SQLLite */
        return self::$pdo ?? (self::$pdo = new PDO(
            'sqlite:'.__DIR__.DIRECTORY_SEPARATOR.'..'.DIRECTORY_SEPARATOR.'..'.DIRECTORY_SEPARATOR.'hw-08.db',
            null,
            null,
            [
                PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION
            ]
        ));
    }
}
