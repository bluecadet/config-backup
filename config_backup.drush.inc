<?php


use Drush\Log\LogLevel;
//use Drupal\Core\Site\Settings;

/**
 * Implementation of hook_drush_command().
 */
function config_backup_drush_command() {
  $deps = array('drupal dependencies' => array('config'));

  $items['config-backup'] = array(
    'description' => 'Export configuration to a backup directory.',
    'core' => array('8+'),
    'aliases' => array('cbkup'),
    'arguments' => array(
      'label' => "A config directory label (i.e. a key in \$config_directories array in settings.php). Defaults to 'sync'",
    ),
    'options' => array(),
    'examples' => array(),
  );

  $items['config-remove-backups'] = array(
    'description' => 'Export configuration to a backup directory.',
    'core' => array('8+'),
    'aliases' => array('crbk'),
    'arguments' => array(
      'label' => "A config directory label (i.e. a key in \$config_directories array in settings.php). Defaults to 'sync'",
    ),
    'options' => array(
      'time' => 'String to use in PHP\'s strtotime() func.',
    ),
    'examples' => array(
      'drush config-remove-backups --time="-2 days"' => 'Remove config backups older than 2 days from right now',
    ),
  );

  $items['config-backup-list'] = array(
    'description' => 'List configuration to a backups.',
    'core' => array('8+'),
    'aliases' => array('cbkl'),
    'arguments' => array(
      'label' => "A config directory label (i.e. a key in \$config_directories array in settings.php). Defaults to 'sync'",
    ),
    'options' => array(),
    'examples' => array(),
  );
  return $items;
}

/**
 * Command callback: Export config to specified directory (usually sync) for backup purposes.
 */
function drush_config_backup($destination = NULL) {
  $settings = Drupal\Core\Site\Settings::get('config_backup_directories', NULL);

  if (!isset($settings['config-backups'])) {
    drush_log('You must set config-backups config directory setting.', LogLevel::ERROR);
    return drush_user_abort();
  }

  $choices = drush_map_assoc(array_keys($settings));

  unset($choices[CONFIG_ACTIVE_DIRECTORY]);
  unset($choices[CONFIG_SYNC_DIRECTORY]);

  if (!isset($destination) && count($choices) >= 2) {
    $destination = drush_choice($choices, 'Choose a destination.');
    if (empty($destination)) {
      return drush_user_abort();
    }
  }
  elseif (!isset($destination)) {
    $destination = 'config-backups';
  }

  $destination_dir = $settings[$destination] . '/' . time();
  $branch = FALSE;

  // Do the actual config export operation
  $result = _drush_config_export($destination, $destination_dir, $branch);

  return TRUE;
}

function drush_config_backup_config_remove_backups($destination = NULL) {
  $settings = Drupal\Core\Site\Settings::get('config_backup_directories', NULL);

  if (!isset($settings['config-backups'])) {
    drush_log('You must set config-backups config directory setting.', LogLevel::ERROR);
    return drush_user_abort();
  }

  $choices = drush_map_assoc(array_keys($settings));

  unset($choices[CONFIG_ACTIVE_DIRECTORY]);
  unset($choices[CONFIG_SYNC_DIRECTORY]);

  if (!isset($destination) && count($choices) >= 2) {
    $destination = drush_choice($choices, 'Choose a destination.');
    if (empty($destination)) {
      return drush_user_abort();
    }
  }
  elseif (!isset($destination)) {
    $destination = 'config-backups';
  }

  $destination_dir = $settings[$destination];

  $time = drush_get_option('time');
  if ($time == NULL) {
    $time = "-2 days";
  }

  $threshold = strtotime($time); // Anything less will be deleted.

  $directories = glob($destination_dir . '/*', GLOB_ONLYDIR);
  foreach ($directories as $dir) {
    $dir_exploded = explode('/', $dir);
    if ($dir_exploded[(count($dir_exploded)-1)] < $threshold) {
      drush_delete_dir($dir);

      drush_log("Removed: " . $dir, LogLevel::SUCCESS);
    }
  }
  drush_log("Finished Cleaning up Config-Backups", LogLevel::SUCCESS);
}

function drush_config_backup_list($destination = NULL) {
  $settings = Drupal\Core\Site\Settings::get('config_backup_directories', NULL);

  if (!isset($settings['config-backups'])) {
    drush_log('You must set config-backups config directory setting.', LogLevel::ERROR);
    return drush_user_abort();
  }

  $choices = drush_map_assoc(array_keys($settings));

  unset($choices[CONFIG_ACTIVE_DIRECTORY]);
  unset($choices[CONFIG_SYNC_DIRECTORY]);

  if (!isset($destination) && count($choices) >= 2) {
    $destination = drush_choice($choices, 'Choose a destination.');
    if (empty($destination)) {
      return drush_user_abort();
    }
  }
  elseif (!isset($destination)) {
    $destination = 'config-backups';
  }
  $destination_dir = $settings[$destination];

  $directories = glob($destination_dir . '/*', GLOB_ONLYDIR);
  foreach ($directories as $dir) {
    drush_log($dir . '/', LogLevel::SUCCESS);
  }

  if (empty($directories)) {
    drush_log("There are no backups", LogLevel::SUCCESS);
  }
}
