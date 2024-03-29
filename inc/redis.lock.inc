<?php

/**
 * @file
 * Backdrop core lock.inc replacement.
 *
 * Do not use this file directly, it will be included by the backend specific
 * implementation when added to settings.php file.
 *
 * See README.md file for details.
 */

// Include our own autoloader to ensure classes to be there.
// We cannot rely on core in case of early bootstrap phases.
require_once dirname(__FILE__) . '/../redis.autoload.inc';

/**
 * This function is here for API consistency and should not be used directly.
 */
function lock_initialize() { }

/**
 * This function is here for API consistency and should not be used directly.
 */
function _lock_id() {
  return Redis_Lock::getBackend()->getLockId();
}

function lock_acquire($name, $timeout = 30.0) {
  return Redis_Lock::getBackend()->lockAcquire($name, $timeout);
}

function lock_may_be_available($name) {
  return Redis_Lock::getBackend()->lockMayBeAvailable($name);
}

function lock_wait($name, $delay = 30) {
  return Redis_Lock::getBackend()->lockWait($name, $delay);
}

function lock_release($name) {
  return Redis_Lock::getBackend()->lockRelease($name);
}

function lock_release_all($lock_id = NULL) {
  return Redis_Lock::getBackend()->lockReleaseAll($lock_id);
}
