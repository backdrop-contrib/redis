<?php

abstract class Redis_Tests_AbstractUnitTestCase extends BackdropUnitTestCase
{
    /**
     * @var boolean
     */
    static protected $loaderEnabled = false;

    /**
     * Enable the autoloader
     *
     * This exists in this class in case the autoloader is not set into the
     * settings.php file or another way
     *
     * @return void|boolean
     */
    static protected function enableAutoload()
    {
        if (self::$loaderEnabled) {
            return;
        }
        if (class_exists('Redis_Client')) {
            return;
        }

        spl_autoload_register(function ($className) {
            $parts = explode('_', $className);
            if ('Redis' === $parts[0]) {
                $filename = __DIR__ . '/../lib/' . implode('/', $parts) . '.php';
                return (bool) include_once $filename;
            }
            return false;
        }, null, true);

        self::$loaderEnabled = true;
    }

    /**
     * Backdrop $settings array backup
     *
     * @var array
     */
    private $originalConf = array(
        'cache_lifetime'          => null,
        'cache_prefix'            => null,
        'redis_client_interface'  => null,
        'redis_eval_enabled'      => null,
        'redis_flush_mode'        => null,
        'redis_perm_ttl'          => null,
    );

    /**
     * Prepare Backdrop environmment for testing
     */
    final private function prepareBackdropEnvironment()
    {
        // Site on which the tests are running may define this variable
        // in their own settings.php file case in which it will be merged
        // with testing site
        global $settings;
        foreach (array_keys($this->originalConf) as $key) {
            if (isset($settings[$key])) {
                $this->originalConf[$key] = $settings[$key];
                unset($settings[$key]);
            }
        }
        $settings['cache_prefix'] = $this->testId;
    }

    /**
     * Restore Backdrop environment after testing.
     */
    final private function restoreBackdropEnvironment()
    {
        $GLOBALS['conf'] = $this->originalConf + $GLOBALS['conf'];
    }

    /**
     * Prepare client manager
     */
    final private function prepareClientManager()
    {
        $interface = $this->getClientInterface();

        if (null === $interface) {
            throw new \Exception("Test skipped due to missing driver");
        }

        $GLOBALS['conf']['redis_client_interface'] = $interface;
        Redis_Client::reset();
    }

    /**
     * Restore client manager
     */
    final private function restoreClientManager()
    {
        Redis_Client::reset();
    }

    /**
     * Set up the Redis configuration.
     *
     * Set up the needed variables using variable_set() if necessary.
     *
     * @return string
     *   Client interface or null if not exists
     */
    abstract protected function getClientInterface();

    /**
     * {@inheritdoc}
     */
    public function setUp()
    {
        self::enableAutoload();

        $this->prepareBackdropEnvironment();
        $this->prepareClientManager();

        parent::setUp();

        backdrop_install_schema('system');
        backdrop_install_schema('locale');
    }

    /**
     * {@inheritdoc}
     */
    public function tearDown()
    {
        backdrop_uninstall_schema('locale');
        backdrop_uninstall_schema('system');

        $this->restoreBackdropEnvironment();
        $this->restoreClientManager();

        parent::tearDown();
    }
}
