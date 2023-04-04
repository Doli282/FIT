#!/usr/bin/python3
from insecure_hash import hash_string
from Crypto.Cipher import AES


def find_collision(message):
    return ""


if __name__ == '__main__':
    message = "aaaaaaaaaaaaaaaabbbbbbbbbbbbbbbb"
    print("Hash of %s is %s" % (message, hash_string(message)))
    collision = find_collision(message)
    print("Hash of %s is %s" % (collision, hash_string(collision)))
