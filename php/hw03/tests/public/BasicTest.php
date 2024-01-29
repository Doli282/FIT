<?php

use PHPUnit\Framework\TestCase;

class BasicTest extends TestCase
{

    public function testBasic(): void
    {
        $bag = new Bag;
        $this->assertSame(true, $bag->isEmpty(), "Basic test failed");
        $this->assertSame(false, $bag->contains('celery'), "Basic test failed");
        $this->assertSame(0, $bag->elementSize('celery'), "Basic test failed");
        $this->assertSame(0, $bag->size(), "Basic test failed");

        $bag->add('celery');
        $this->assertSame(false, $bag->isEmpty(), "Basic test failed");
        $this->assertSame(true, $bag->contains('celery'), "Basic test failed");
        $this->assertSame(1, $bag->elementSize('celery'), "Basic test failed");
        $this->assertSame(1, $bag->size(), "Basic test failed");

        $bag->add('celery');
        $bag->add('celery');
        $bag->add('celery');
        $this->assertSame(4, $bag->elementSize('celery'), "Basic test failed");
        $this->assertSame(4, $bag->size(), "Basic test failed");
        $this->assertSame(false, $bag->isEmpty(), "Basic test failed");

        $bag->add('eggplant');
        $bag->add('eggplant');
        $this->assertSame(4, $bag->elementSize('celery'), "Basic test failed");
        $this->assertSame(2, $bag->elementSize('eggplant'), "Basic test failed");
        $this->assertSame(6, $bag->size(), "Basic test failed");
        $this->assertSame(false, $bag->isEmpty(), "Basic test failed");

        $bag->add('parsley');
        $this->assertSame(4, $bag->elementSize('celery'), "Basic test failed");
        $this->assertSame(2, $bag->elementSize('eggplant'), "Basic test failed");
        $this->assertSame(1, $bag->elementSize('parsley'), "Basic test failed");
        $this->assertSame(7, $bag->size(), "Basic test failed");
        $this->assertSame(false, $bag->isEmpty(), "Basic test failed");

        $bag->remove('parsley');
        $this->assertSame(4, $bag->elementSize('celery'), "Basic test failed");
        $this->assertSame(2, $bag->elementSize('eggplant'), "Basic test failed");
        $this->assertSame(0, $bag->elementSize('parsley'), "Basic test failed");
        $this->assertSame(6, $bag->size(), "Basic test failed");
        $this->assertSame(false, $bag->isEmpty(), "Basic test failed");

        $bag->remove('celery');
        $this->assertSame(3, $bag->elementSize('celery'), "Basic test failed");
        $this->assertSame(2, $bag->elementSize('eggplant'), "Basic test failed");
        $this->assertSame(0, $bag->elementSize('parsley'), "Basic test failed");
        $this->assertSame(5, $bag->size(), "Basic test failed");
        $this->assertSame(false, $bag->isEmpty(), "Basic test failed");

        $bag->remove('celery');
        $bag->remove('celery');
        $bag->remove('celery');
        $bag->remove('celery');
        $bag->remove('celery');
        $bag->remove('celery');
        $this->assertSame(0, $bag->elementSize('celery'), "Basic test failed");
        $this->assertSame(2, $bag->elementSize('eggplant'), "Basic test failed");
        $this->assertSame(0, $bag->elementSize('parsley'), "Basic test failed");
        $this->assertSame(2, $bag->size(), "Basic test failed");
        $this->assertSame(false, $bag->isEmpty(), "Basic test failed");

        $bag->clear();
        $this->assertSame(0, $bag->elementSize('celery'), "Basic test failed");
        $this->assertSame(0, $bag->elementSize('eggplant'), "Basic test failed");
        $this->assertSame(0, $bag->elementSize('parsley'), "Basic test failed");
        $this->assertSame(0, $bag->size(), "Basic test failed");
        $this->assertSame(true, $bag->isEmpty(), "Basic test failed");

        $set = new Set;

        $this->assertInstanceOf(Bag::class, $set, "Basic test failed");
        $this->assertSame(true, $set->isEmpty(), "Basic test failed");
        $this->assertSame(false, $set->contains('celery'), "Basic test failed");
        $this->assertSame(0, $set->elementSize('celery'), "Basic test failed");
        $this->assertSame(0, $set->size(), "Basic test failed");

        $set->add('celery');
        $this->assertSame(true, $set->contains('celery'), "Basic test failed");
        $this->assertSame(1, $set->elementSize('celery'), "Basic test failed");
        $this->assertSame(1, $set->size(), "Basic test failed");
        $this->assertSame(false, $set->isEmpty(), "Basic test failed");

        $set->add('celery');
        $set->add('celery');
        $set->add('celery');
        $this->assertSame(1, $set->elementSize('celery'), "Basic test failed");
        $this->assertSame(1, $set->size(), "Basic test failed");

        $set->add('eggplant');
        $set->add('eggplant');
        $this->assertSame(1, $set->elementSize('eggplant'), "Basic test failed");
        $this->assertSame(1, $set->elementSize('celery'), "Basic test failed");
        $this->assertSame(2, $set->size(), "Basic test failed");

        $set->add('parsley');
        $this->assertSame(1, $set->elementSize('eggplant'), "Basic test failed");
        $this->assertSame(1, $set->elementSize('parsley'), "Basic test failed");
        $this->assertSame(1, $set->elementSize('celery'), "Basic test failed");
        $this->assertSame(3, $set->size(), "Basic test failed");

        $set->remove('parsley');
        $this->assertSame(1, $set->elementSize('eggplant'), "Basic test failed");
        $this->assertSame(0, $set->elementSize('parsley'), "Basic test failed");
        $this->assertSame(1, $set->elementSize('celery'), "Basic test failed");
        $this->assertSame(2, $set->size(), "Basic test failed");

        $set->remove('celery');
        $this->assertSame(1, $set->elementSize('eggplant'), "Basic test failed");
        $this->assertSame(0, $set->elementSize('parsley'), "Basic test failed");
        $this->assertSame(0, $set->elementSize('celery'), "Basic test failed");
        $this->assertSame(1, $set->size(), "Basic test failed");

        $set->remove('celery');
        $set->remove('celery');
        $set->remove('celery');
        $set->remove('celery');
        $set->remove('celery');
        $set->remove('celery');
        $this->assertSame(1, $set->elementSize('eggplant'), "Basic test failed");
        $this->assertSame(0, $set->elementSize('parsley'), "Basic test failed");
        $this->assertSame(0, $set->elementSize('celery'), "Basic test failed");
        $this->assertSame(1, $set->size(), "Basic test failed");

        $set->clear();
        $this->assertSame(0, $set->elementSize('eggplant'), "Basic test failed");
        $this->assertSame(0, $set->elementSize('parsley'), "Basic test failed");
        $this->assertSame(0, $set->elementSize('celery'), "Basic test failed");
        $this->assertSame(0, $set->size(), "Basic test failed");
    }
}
