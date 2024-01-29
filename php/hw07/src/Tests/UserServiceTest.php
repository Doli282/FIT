<?php declare(strict_types=1);

namespace HW\Tests;

use HW\Factory\UserServiceFactory;
use HW\Interfaces\IStorage;
use PHPUnit\Framework\TestCase;

class UserServiceTest extends TestCase
{
    private function getUserService(IStorage $storage) {
        return UserServiceFactory::get($storage);
    }

    // ----- test createUser -----
    /** @dataProvider _newUserProvider */
    public function testCreateUser($uname, $mail) : void
    {
        $stub = $this->createStub(IStorage::class);
        $stub->method('save')->willReturn(null);
        $service = $this->getUserService($stub);
        $res = $service->createUser($uname, $mail);
        $this->assertIsString($res);
        $this->assertNotEmpty($res);
    }

    /** @depends testCreateUser
     *  @dataProvider _newUserProvider
     */
    public function testCreateUserUnique($username, $email) : void
    {
        $uname = "uname";
        $mail = "email@example.com";
        $stub = $this->createStub(IStorage::class);
        $stub->method('save')->willReturn(null);
        $service = $this->getUserService($stub);

        $res = $service->createUser($uname, $mail);
        $res2 = $service->createUser($username, $email);
        $this->assertNotEquals($res, $res2);
    }

    /** @dataProvider _invalidString */
    public function testFailingCreateUserUsernameInvalidString($username) : void
    {
        $mail = "email@example.com";
        $stub = $this->createStub(IStorage::class);
        $service = $this->getUserService($stub);
        $this->expectException(\InvalidArgumentException::class);
        $service->createUser($username, $mail);
    }

    /** @dataProvider _invalidString */
    public function testFailingCreateUserEmailInvalidString($email) : void
    {
        $uname = "uname";
        $stub = $this->createStub(IStorage::class);
        $service = $this->getUserService($stub);
        $this->expectException(\InvalidArgumentException::class);
        $service->createUser($uname, $email);
    }

    /** @dataProvider _invalidEmail */
    public function testFailingCreateUserEmailNotAMailAddress($email) : void
    {
        $uname = "uname";
        $stub = $this->createStub(IStorage::class);
        $service = $this->getUserService($stub);
        $this->expectException(\InvalidArgumentException::class);
        $service->createUser($uname, $email);
    }

    // ----- test getUsername -----
    /** @dataProvider _userProvider */
    public function testGetUsername($id, $username, $email, $json) : void
    {
        $stub = $this->createStub(IStorage::class);
        $stub->method('get')->willReturn($json);
        $service = $this->getUserService($stub);
        $result = $service->getUsername($id);
        $this->assertNotNull($result);
        $this->assertEquals($username, $result);
    }

    public function testGetUsernameUnknownUser() : void
    {
        $stub = $this->createStub(IStorage::class);
        $stub->method('get')->willReturn(null);
        $service = $this->getUserService($stub);
        $result = $service->getUsername("id");
        $this->assertNull($result);
    }

    /** @dataProvider _notAStringProvider */
    public function testFailingGetUsernameInvalidID($id) : void
    {
        $stub = $this->createStub(IStorage::class);
        $stub->method('get')->willReturn(null);
        $service = $this->getUserService($stub);
        $this->expectException(\InvalidArgumentException::class);
        $service->getUsername($id);
    }

    /** @depends testGetUsername */
    public function testGetUsernameAccordingToID()
    {
        $username1 = "uname1";
        $email1 = "mail1@example.com";
        $id1 = "a1";
        $json1 = json_encode([
            'username' => $username1,
            'email' => $email1
        ], JSON_THROW_ON_ERROR);

        $username2 = "uname2";
        $email2 = "mail2@example.com";
        $id2 = "a2";
        $json2 = json_encode([
            'username' => $username2,
            'email' => $email2
        ], JSON_THROW_ON_ERROR);

        $storage = $this->createMock(IStorage::class);
        $storage->method('get')->willReturnMap([
            [$id1, $json1],
            [$id2, $json2]
        ]);
        $service = $this->getUserService($storage);

        $result1 = $service->getUsername($id1);
        $result2 = $service->getUsername($id2);
        $this->assertNotEquals($result1, $result2);
    }

    // ----- test getEmail -----
    /** @dataProvider _userProvider */
    public function testGetEmail($id, $username, $email, $json) : void
    {
        $stub = $this->createStub(IStorage::class);
        $stub->method('get')->willReturn($json);
        $service = $this->getUserService($stub);
        $result = $service->getEmail($id);
        $this->assertNotNull($result);
        $this->assertEquals($email, $result);
    }

    public function testGetEmailUnknownUser() : void
    {
        $stub = $this->createStub(IStorage::class);
        $stub->method('get')->willReturn(null);
        $service = $this->getUserService($stub);
        $result = $service->getEmail("id");
        $this->assertNull($result);
    }

    /** @dataProvider _notAStringProvider */
    public function testFailingGetEmailInvalidID($id) : void
    {
        $stub = $this->createStub(IStorage::class);
        $stub->method('get')->willReturn(null);
        $service = $this->getUserService($stub);
        $this->expectException(\InvalidArgumentException::class);
        $service->getEmail($id);
    }

    /** @depends testGetEmail */
    public function testGetEmailAccordingToID()
    {
        $username1 = "uname1";
        $email1 = "mail1@example.com";
        $id1 = "a1";
        $json1 = json_encode([
            'username' => $username1,
            'email' => $email1
        ], JSON_THROW_ON_ERROR);

        $username2 = "uname2";
        $email2 = "mail2@example.com";
        $id2 = "a2";
        $json2 = json_encode([
            'username' => $username2,
            'email' => $email2
        ], JSON_THROW_ON_ERROR);

        $storage = $this->createMock(IStorage::class);
        $storage->method('get')->willReturnMap([
           [$id1, $json1],
           [$id2, $json2]
        ]);
        $service = $this->getUserService($storage);

        $result1 = $service->getEmail($id1);
        $result2 = $service->getEmail($id2);
        $this->assertNotEquals($result1, $result2);
    }


    // ---- Providers ----
    private function _newUserProvider() : \Generator
    {
        $username = "   uname    ";
        $mail = "      email@example.com    ".PHP_EOL;
        yield[$username, $mail];
        $username = "uname";
        $mail = "email@example.com";
        yield[$username, $mail];
        for($i = 0; $i < 5; $i++)
        {
            $username = $username . $i;
            $mail = $i . $mail;
            yield [$username, $mail];
        }
    }
    private function _userProvider() : array
    {
        $username = "uname";
        $email = "mail@example.com";
        $id = "a1";
        $json = json_encode([
            'username' => $username,
            'email' => $email
        ], JSON_THROW_ON_ERROR);
        return [
            [$id, $username, $email, $json]
        ];
    }

    private function _notAStringProvider() : array
    {
        return [
            [null], [1],  [5.2]
        ];
    }

    private function _invalidString() : array
    {
        $array = [[""], ["        "]];
        return array_merge($array, $this->_notAStringProvider());
    }
    private function _invalidEmail() : array
    {
        return [
            ["mail"],
            ["mail.com"],
            ["mail@example"],
            ["mail@example@example.com"],
            ["mai l@example.com"],
            ["first.last@example.123"],
            ["first.last@com"],
            ['"Fred Bloggs"@iana.org'],
            ['first.last@[IPv6:1111:2222:3333::4444:5555:12.34.56.78]'],
            ['first.last@[IPv6:1111:2222:3333::4444:5555:6666:7777]'],
            ['"[[ test ]]"@iana.org'],
            ['first().last@iana.org']
        ];
    }
}
