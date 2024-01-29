<?php

namespace Public;

use PHPUnit\Framework\TestCase;
use PHPUnit\TextUI\Command;
use SebastianBergmann\CodeCoverage\CodeCoverage;

class TestSuccessAndCodeCoverageTest extends TestCase
{
    private const SRC_PATH = "../../src/Lib/";
    private const TEST_PATH = __DIR__ . "/../../src/Tests";
    private const TESTED_FILES = [
        self::SRC_PATH . "LinkedList.php",
        self::SRC_PATH . "LinkedListItem.php",
        self::SRC_PATH . "MathUtils.php",
        self::SRC_PATH . "UserService.php"
    ];
    public function testSACC() {
        $tester = new Command();
        $c = __DIR__ . "/coverage.php";
        ob_start();
        $result = $tester->run(["--coverage-php", $c, "--colors=never", self::TEST_PATH], false);
        $this->assertEquals(0, $result, "Student tests fails on student solution");
        ob_end_clean();
        /** @var CodeCoverage $coverage */
        $coverage = include $c;
        unlink($c);
        $lineCoverage = $coverage->getData()->lineCoverage();
        foreach (self::TESTED_FILES as $testedFile) {
            $path = realpath(__DIR__ . "/" . $testedFile);
            $this->assertArrayHasKey($path, $lineCoverage, "File '". $testedFile . "' has no test on it.");
            $lines = $lineCoverage[$path];
            foreach ($lines as $line => $te) {
                $this->assertNotEmpty($te, "Line '" . $line . "' in file '". $testedFile . "' isn't covered.");
            }
        }
    }
}