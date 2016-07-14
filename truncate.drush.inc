<?php

/**
 * Implements hook_drush_command().
 *
 * @return mixed
 */
function truncate_drush_command() {
  $items['truncate'] = array(
    'description' => 'Truncate one or a series of database tables.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_DATABASE,
    'arguments' => array(
      'tables' => 'A single table or comma separated list.  An asterisk (*) can be used as a wildcard.',
    ),
    'options' => array(
      'interactive' => dt("Interactive mode.  Confirm each table before truncating."),
    ),
    'examples' => array(
      'drush truncate cache_*' => dt("Truncate all the tables that begin with 'cache_'."),
    ),
    'required-arguments' => TRUE,
  );

  return $items;
}

/**
 * Command callback.
 */
function drush_truncate($table_names) {
  $truncate = array();
  $dne = array();

  // Resolve the site alias.
  $alias = drush_get_context(DRUSH_TARGET_SITE_ALIAS, '@self');
  $alias = empty($alias) ? '@self' : $alias;

  // Get the DB name.
  list($site_list,) = drush_sitealias_resolve_sitespecs(array($alias));
  $site = reset($site_list);
  drush_sitealias_add_db_settings($site);
  $db = $site['databases']['default']['default']['database'];

  // If we can't resolve a DB name, kick out.
  if (!$db) {
    return drush_set_error('NODB', dt('Unable to determine a database for this site.'));
  }

  // First, split the table string.
  $tables = explode(',', $table_names);
  foreach ($tables as $table) {
    // Check for wildcards and lookup all tables that match the pattern.
    if (strstr($table, '*')) {
      $table_like = str_replace('*', '%', $table);
      $result = db_query("SELECT TABLE_NAME FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = :db AND TABLE_NAME LIKE :table;", array(':db' => $db, ':table' => $table_like));

      foreach ($result as $row) {
        $truncate[] = $row->TABLE_NAME;
      }
    }
    else {
      if (!db_table_exists($table)) {
        $dne[] = $table;
      }
      else {
        $truncate[] = $table;
      }
    }
  }

  // If there are erroneous tables report them.
  if (!empty($dne)) {
    $error = drush_set_error('TABLE_DNE', dt('The following tables do not exist on database @db and cannot be truncated.', array('@db' => $db)));
    drush_print_format($dne, 'list');
    return $error;
  }

  // Print a list of tables to be truncated.
  drush_print_format($truncate, 'list');

  // See if we have an option for interactive mode.
  $interactive = drush_get_option('interactive', FALSE);

  // Make sure the user is cool with this.
  foreach ($truncate as $table) {
    $args = array('@db' => $db, '@table' => $table);

    // Confirm each truncation.
    if ($interactive) {
      if (drush_confirm(dt("Truncate @db.@table?", $args))) {
        db_query("TRUNCATE TABLE {{$table}}");
      }
      else {
        drush_print(dt("Skipped @db.@table.", $args));
      }
    }
    // Confirm the process as a whole.
    elseif (drush_confirm(dt("You are about to truncate !num tables on !db. Continue?", array('!num' => count($truncate), '!db' => $db)))) {
      db_query("TRUNCATE TABLE {{$table}}");
      drush_print(dt('Truncated @db.@table', $args));
    }
    // Kick out.
    else {
      return drush_user_abort();
    }
  }
}
