<?php

namespace HW\Interfaces;

use InvalidArgumentException;
use JsonException;

interface IUserService
{

    /**
     * @param IStorage $storage storage used for storing Users
     */
    public function __construct(IStorage $storage);

    /**
     * @param $username mixed of user, whitespace characters at begin and end will be ignored
     * @param $email mixed of user, whitespace characters at begin and end will be ignored
     * @return string unique id assigned to user
     * @throws InvalidArgumentException when username / email aren't string or are only whitespace characters or are empty or email isn't valid email address
     */
    public function createUser($username, $email);

    /**
     * gets username for user by id
     * @param $id mixed id of user
     * @return string|null username or null if not found
     * @throws InvalidArgumentException when $id is not string
     */
    public function getUsername($id);

    /**
     * gets email for user by id
     * @param $id mixed id of user
     * @return string|null email or null if not found
     * @throws InvalidArgumentException when $id is not string
     */
    public function getEmail($id);
}