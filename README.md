Redis cache backends
====================

This package provides two different Redis cache backends. If you want to use
Redis as cache backend, you have to choose one of the two, but you cannot use
both at the same time.

Installation
------------

Install this module using the official Backdrop CMS instructions at <https://backdropcms.org/guide/modules>.


Predis
------

This implementation uses the Predis PHP library. It is compatible with PHP 5.3
only.

PhpRedis
--------

This implementation uses the PhpRedis PHP extension. In order to use it, you
will need to compile the extension.

Redis version
-------------

This module requires Redis version to be 2.6.0 or later with LUA scripting
enabled due to the EVAL command usage.

If you cannot upgrade your Redis server, use the module versions as defined
below:

  * 3.x release will only officially support Redis server <= 2.8 and 3.x
    This branch will work with Redis 2.4 if you configure your cache
    backend to operate in sharding mode.

  * For Redis 2.4, use the latest 2.x release of this module or use the
    3.x release with sharding mode enabled.

  * For Redis <=2.3, use any version of this module <=2.6.

Notes
-----

Both backends provide the exact same functionalities. The major difference is
because PhpRedis uses a PHP extension, and not PHP code, it will perform a
lot better (Predis needs PHP userland code to be loaded).

The difference between the two backends was tested to be a few milliseconds on
local author testing. In case you attempt to profile the code, traces will be a
lot bigger.

Note that most of the settings are shared. See next sections.

Getting started
===============

Quick setup
-----------

Here is a simple yet working easy way to setup the module.
This method will allow Backdrop to use Redis for all caches and locks
and path alias cache replacement.

    $settings['redis_client_interface'] = 'PhpRedis'; // Can be "Predis".
    $settings['redis_client_host']      = '1.2.3.4';  // Your Rdis instance hostname or IP address.
    $settings['lock_inc']               = 'modules/redis/redis.lock.inc';
    $settings['path_inc']               = 'modules/redis/redis.path.inc';
    $settings['cache_backends'][]       = 'modules/redis/redis.autoload.inc';
    $settings['cache_default_class']    = 'Redis_Cache';

For more detailed setup, see the below sections.

Are there any cache bins that should *never* go into Redis?
----------------------------------------------------------

No. Redis has been maturing a lot over time, and will apply different sensible
settings for different bins.

Advanced configuration
======================

Use the compressed cache
------------------------

Please note this is (for now) an experimental feature. As a personnal note
from the module author, it should be safe to use.

Use this cache class setting to enable compression. This will save usually
about 80% RAM at the cost of some milliseconds server time.

    $settings['cache_default_class'] = 'Redis_CacheCompressed';

Additionally, you can alter the default size compression threshold, under which
entries will not be compressed (size is in bytes, set 0 to always compress):

    $settings['cache_compression_size_threshold'] = 100;

You can also change the compression level, which an positive integer between
1 and 9, 1 being the lowest but fastest compression ratio, 9 being the most
aggressive compression but is a lot slower. From testing, setting it to the
lower level (1) gives 80% memory usage decrease, which is more than enough.

    $settings['cache_compression_ratio'] = 5;

Please note that those settings are global and not on a cache bin basis, you can
already control whenever the compression is to be used or not by selecting a
different cache class on per cache bin basis.

If you switch from the standard default backend (without compression) to the
compressed cache backend, it will recover transparently uncompressed data and
proceed normally without additional cache eviction, it safe to upgrade.
Donwgrading from compressed data to uncompressed data won't work, but the
cache backend will just give you cache hit miss and it will work seamlessly
too without any danger for the site.

Choose the Redis client library to use
--------------------------------------

Add into your settings.php file:

    $settings['redis_client_interface']      = 'PhpRedis';

You can replace 'PhpRedis' with 'Predis', depending on the library you chose.

Note that this is optional but recommended. If you do not set this variable, the
module will proceed to class lookups and attempt to choose the best client
available (with a preference for the Predis one).

Tell Backdrop to use the cache backend
------------------------------------

To use the standard cache backend configuration, update your settings.php file
with:

    $settings['cache_backends'][]            = 'modules/redis/redis.autoload.inc';
    $settings['cache_class_cache']           = 'Redis_Cache';
    $settings['cache_class_cache_menu']      = 'Redis_Cache';
    $settings['cache_class_cache_bootstrap'] = 'Redis_Cache';
    // ... Any other bins.

Tell Backdrop to use the lock backend
-----------------------------------

To use the standard lock backend override, update your settings.php file with:

    $settings['lock_inc'] = 'modules/redis/redis.lock.inc';

Tell Backdrop to use the path alias backend
-----------------------------------------

Too use the standard path backend override, update your settings.php file
with:

    $settings['path_inc'] = 'modules/redis/redis.path.inc';

Notice that there is an additional variable for path handling that is set
per default, which will ignore any path that is an admin path (which gains a few
SQL queries). If you want to be able to set aliases on admin path and restore
an almost default Backdrop core behavior, you should add this line into your
settings.php file:

    $settings['path_alias_admin_blacklist'] = FALSE;

Common settings
===============

Connect through a UNIX socket
------------------------------

All you have to do is specify this line:

    $settings['redis_client_socket'] = '/some/path/redis.sock';

Both drivers support it.

Connect to a remote host
------------------------

If your Redis instance is remote, you can use this syntax:

    $settings['redis_client_host'] = '1.2.3.4';
    $settings['redis_client_port'] = 1234;

Port is optional, default is 6379 (default Redis port).

Using a specific database
-------------------------

Per default, Redis ships the database "0". All default connections will be use
this one if nothing is specified.

Depending on your OS or OS distribution, you might have numerous databases. To
use one in particular, just add to your settings.php file:

    $settings['redis_client_base'] = 12;

Please note that if you are working in shard mode, you should never set this
variable.

Connection to a password protected instance
-------------------------------------------

If you are using a password protected instance, specify the password this way:

    $settings['redis_client_password'] = "mypassword";

Depending on the backend, using a wrong auth will behave differently:

 - Predis will throw an exception and make Backdrop fail during early boostrap.

 - PhpRedis will make Redis calls silent and creates some PHP warnings, thus
   Backdrop will behave as if it was running with a null cache backend (no cache
   at all).

Prefixing site cache entries (avoiding sites name collision)
------------------------------------------------------------

If you need to differentiate multiple sites using the same Redis instance and
database, you will need to specify a prefix for your site cache entries.

Important note: most people do not need that feature since that when no prefix
is specified, the Redis module will attempt to use the a hash of the database
credentials in order to provide a multisite safe default behavior. This means
that the module will also safely work in CLI scripts.

Cache prefix configuration attempts to use a unified variable accross contrib
backends that support this feature. This variable name is 'cache_prefix'.

This variable is polymorphic, the simplest version is to provide a raw string
that will be the default prefix for all cache bins:

    $settings['cache_prefix'] = 'mysite_';

Alternatively, to provide the same functionality, you can provide the variable
as an array:

    $settings['cache_prefix']['default'] = 'mysite_';

This allows you to provide different prefix depending on the bin name. Common
usage is that each key inside the 'cache_prefix' array is a bin name, the value
the associated prefix. If the value is explicitly FALSE, then no prefix is
used for this bin.

The 'default' meta bin name is provided to define the default prefix for non
specified bins. It behaves like the other names, which means that an explicit
FALSE will order the backend not to provide any prefix for any non specified
bin.

Here is a complex sample:

    // Default behavior for all bins, prefix is 'mysite_'.
    $settings['cache_prefix']['default'] = 'mysite_';

    // Set no prefix explicitely for 'cache' and 'cache_bootstrap' bins.
    $settings['cache_prefix']['cache'] = FALSE;
    $settings['cache_prefix']['cache_bootstrap'] = FALSE;

    // Set another prefix for 'cache_menu' bin.
    $settings['cache_prefix']['cache_menu'] = 'menumysite_';

Note that this last notice is Redis only specific, because per default Redis
server will not namespace data, thus sharing an instance for multiple sites
will create conflicts. This is not true for every contributed backends.

Sharding vs normal mode
-----------------------

Per default the Redis cache backend will be in "normal" mode, meaning that
every flush call will trigger and EVAL lua script that will proceed to cache
wipeout and cleanup the Redis database from stalled entries.

Nevertheless, if you are working with a Redis server < 2.6 or in a sharded
environment, you cannot multiple keys per command nor proceed to EVAL'ed
scripts, you will then need to switch to the sharded mode.

Sharded mode will never delete entries on flush calls, but register a key
with the current flush time instead. Cache entries will then be deleted on
read if the entry checksum does not match or is older than the latest flush
call. Note that this mode is fast and safe, but must be used accordingly
with the default lifetime for permanent items, else your Redis server might
keep stalled entries into its database forever.

In order to enable the sharded mode, set into your settings.php file:

    $settings['redis_flush_mode'] = 3;

Please note that the value 3 is there to keep backward compatibility with
older versions of the Redis module and will not change.

Note that previous Redis module version allowed to set a per-bin setting for
the clear mode value; nevertheless the clear mode is not a valid setting
anymore and the past issues have been resolved. Only the global value will
work as of now.

Sharding and pipelining
-----------------------

When using this module with sharding mode you may have a sharding proxy able to
do command pipelining. If that is the case, you should switch to "sharding with
pipelining" mode instead:

    $settings['redis_flush_mode'] = 4;

Note that if you use the sharding mode because you use an older version of the
Redis server, you should always use this mode to ensure the best performances.

Default lifetime for permanent items
------------------------------------

Redis when reaching its maximum memory limit will stop writing data in its
storage engine: this is a feature that avoid the Redis server crashing when
there is no memory left on the machine.

As a workaround, Redis can be configured as a LRU cache for both volatile or
permanent items, which means it can behave like Memcache. Problems arise if
you use Redis as a permanent storage for other business matters beyond this
module, you cannot possibly configure it to drop permanent items or you will
risk losing data.

This workaround allows you to explicitly set a very long or configured default
lifetime for CACHE_PERMANENT items (that would normally be permanent) which
will mark them as being volatile in Redis storage engine. This then allows you
to configure a LRU behavior for volatile keys without engaging the permanent
business stuff in a dangerous LRU mechanism. Unused cache items, even if
permanent, will be dropped using this workaround.

Per default, the TTL for permanent items will set to safe-enough value which is
one year; No matter how Redis will be configured default configuration or lazy
admin will inherit from a safe module behavior with zero-conf.

For adventurous users, you can manage the TTL on a per bin basis and change
the default one:

    // Make CACHE_PERMANENT items being permanent once again
    // 0 is a special value usable for all bins to explicitly tell the
    // cache items will not be volatile in Redis.
    $settings['redis_perm_ttl'] = 0;

    // Make them being volatile with a default lifetime of 1 year.
    $settings['redis_perm_ttl'] = "1 year";

    // You can override on a per-bin basis;
    // For example make cached field values live only 3 months:
    $settings['redis_perm_ttl_cache_field'] = "3 months";

    // But you can also put a timestamp in there; In this case the
    // value must be a STRICTLY TYPED integer:
    $settings['redis_perm_ttl_cache_field'] = 2592000; // 30 days.

The time interval string is parsed using DateInterval::createFromDateString.
For more information, please refer to its documentation:

* http://www.php.net/manual/en/dateinterval.createfromdatestring.php

Please also be careful about the fact that those settings are overridden by
the 'cache_lifetime' Backdrop variable, which should always be set to 0.
Moreover, this setting will affect all cache entries without exception so
be careful and never set values too low if you don't want this setting to
override default expire value given by modules on temporary cache entries.

Lock backends
-------------

Both implementations provide a Redis lock backend. Redis lock backend proved to
be faster than the default SQL based one when using both servers on the same
box.

Both backends, thanks to the Redis WATCH, MULTI and EXEC commands, provide
real race condition free mutexes by using Redis transactions.

Queue backend
-------------

This module provides an experimental queue backend. It is for now implemented
only using the PhpRedis driver, any attempt to use it using Predis will result
in runtime errors.

If you want to change the queue driver system wide, set this into your
setting.php file:

    $settings['queue_default_class'] = 'Redis_Queue';
    $settings['queue_default_reliable_class'] = 'Redis_Queue';

Note that some queue implementations, such as the batch queue, are hardcoded
within Backdrop and will always use a database dependent implementation.

If you need to proceed with finer tuning, you can set a per-queue class in
such way:

    $settings['queue_class_NAME'] = 'Redis_Queue';

Where NAME is the arbitrary module given queue name, used as first parameter
for the method BackdropQueue::get().

THIS IS STILL VERY EXPERIMENTAL. The queue should work without any problems
except it does not implement the item lease time correctly, this means that
items that are too long to process won't be released back and forth but will
block the thread processing it instead. This is the only side effect I am
aware of at the current time.

Failover, sharding and partitioning
===================================

Important notice
----------------

There are numerous support and feature request issues about client sharding,
failover ability, multi-server connection, ability to read from slave and
server clustering opened in the issue queue. Note that there is not one
universally efficient solution for this: most of the solutions require that
you cannot use the MULTI/EXEC command using more than one key, and that you
cannot use complex UNION and intersection features anymore.

This module does not implement any kind of client side key hashing or sharding
and never intended to. We recommend that you read the official Redis
documentation page about partitioning.

The best solution for clustering and sharding today seems to be the proxy
assisted partitioning using tools such as Twemproxy.

Current components state
------------------------

As of now, provided components are simple enough so they never use WATCH or
MULTI/EXEC transaction blocks on multiple keys. This means that you can use
them in an environment doing data sharding/partitioning. This remains true
except when you use a proxy that blocks those commands such as Twemproxy.

Lock
----

Lock backend works on a single key per lock, it theoretically guarantees the
atomicity of operations therefore is usable in a sharded environment. Sadly
if you use proxy assisted sharding such as Twemproxy, WATCH, MULTI and EXEC
commands won't pass making it non shardable.

Path
----

Path backend is not used on transactions, it is safe to use in a sharded
environment. Note that this backend uses a single HASH key per language
and per way (alias to source or source to alias) and therefore won't benefit
greatly if not at all from being sharded.

Cache
-----

Cache uses pipelined transactions but does not uses it to guarantee any kind
of data consistency. If you use a smart sharding proxy it is supposed to work
transparently without any hiccups.

Queue
-----

Queue is still in development. There might be problems in the long term for
this component in sharded environments.

Issues
------

To submit bug reports and feature suggestions, or to track changes:
  https://github.com/backdrop-contrib/redis/issues

Current Maintainers
-------------------

- [Joseph Flatt](https://github.com/hosef/)

Credits
-------

- Ported to Backdrop by [Joseph Flatt](https://github.com/hosef/)
- Originally developed for Drupal by [pounard](https://www.drupal.org/u/pounard)
- Currently maintained for Drupal by [Berdir](https://www.drupal.org/u/berdir).
- Many more credits are listed on the [Drupal.org page](https://www.drupal.org/project/redis)

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.
