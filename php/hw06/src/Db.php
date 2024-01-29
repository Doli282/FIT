<?php declare(strict_types=1);

namespace App;

use PDO;

class Db
{
    protected static ?PDO $pdo = null;

    public static function get(): PDO
    {
//        return self::$pdo ?? (self::$pdo = new PDO(
//            'pgsql:host=postgres;port=5432;dbname=devdb;',  // connection string format: driver://host:port/database_name
//            "dev",
//            "pass",
//            [
//                PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION
//            ]
//        ));

        /** For SQLLite */
        return self::$pdo ?? (self::$pdo = new PDO(
            'sqlite:hw-06.db',
            null,
            null,
            [
                PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION
            ]
        ));
    }
}
