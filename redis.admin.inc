<?php

/**
 * @file
 * Redis module administration pages.
 */

/**
 * Main settings and review administration screen.
 */
function redis_settings_form($form, &$form_state) {
  $config = config('redis.settings');

  $form['#config'] = 'redis.settings';
  $form['connection'] = array(
    '#type' => 'fieldset',
    '#title' => t("Connection information"),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['connection']['description'] = array(
    '#markup' => t('These settings are used to setup the connection to the Redis server. They will be ignored if the settinsgs are overridden in settings.php.'),
  );

  $form['connection']['scheme'] = array(
    '#type' => 'textfield',
    '#title' => t("Scheme"),
    '#default_value' => 'tcp',
    '#disabled' => TRUE,
    '#description' => t("Connection scheme.") . " " . t("Only <em>tcp</em> is currently supported. This is ignored when using a UNIX socket."),
  );
  $form['connection']['redis_client_host'] = array(
    '#type' => 'textfield',
    '#title' => t("Host"),
    '#default_value' => $config->get('redis_client_host'),
    '#description' => t("Redis server host. Default is <em>@default</em>.", array('@default' => Redis_Client_Manager::REDIS_DEFAULT_HOST)),
  );
  $form['connection']['redis_client_port'] = array(
    '#type' => 'textfield',
    '#title' => t("Port"),
    '#default_value' => $config->get('redis_client_port'),
    '#description' => t("Redis server port. Default is <em>@default</em>.", array('@default' => Redis_Client_Manager::REDIS_DEFAULT_PORT)),
  );
  $form['connection']['redis_client_socket'] = array(
    '#type' => 'textfield',
    '#title' => t("UNIX socket"),
    '#default_value' => $config->get('redis_client_socket'),
    '#description' => t("Redis UNIX socket for connection. If set remote server host and port will be ignored."),
  );
  $form['connection']['redis_client_base'] = array(
    '#type' => 'textfield',
    '#title' => t("Database"),
    '#default_value' => $config->get('redis_client_base'),
    '#description' => t("Redis server database. Default is none, Redis server will autoselect the database 0."),
  );
  $form['connection']['redis_client_interface'] = array(
    '#type' => 'radios',
    '#title' => t("Client"),
    '#options' => array(
      NULL => t("None or automatic"),
      'PhpRedis' => t("PhpRedis PHP extension"),
      'Predis' => t("Predis PHP library"),
    ),
    '#default_value' => $config->get('redis_client_interface'),
    '#description' => t("Redis low level backend."),
  );

  $form = system_settings_form($form);

  return $form;
}
