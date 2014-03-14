<?php
/**
 * @file remote_entity.admin.inc
 *
 * Contains admin page callbacks.
 */

/**
 * Admin page callback to show a summary of remote entities.
 */
function remote_entity_page_report() {
  $output = '';

  // Show a summary of entities needing remote save.
  $needs_remote_save_list = array();
  foreach (entity_get_info() as $entity_type => $entity_info) {
    // Skip entities that are not remote entities.
    if (!isset($entity_info['remote base table'])) {
      continue;
    }

    // Skip entities that don't have the 'needs remote save' boolean.
    if (!isset($entity_info['entity keys']['needs remote save'])) {
      continue;
    }

    $table = $entity_info['base table'];
    $id_key = $entity_info['entity keys']['id'];
    $needs_remote_save_key  = $entity_info['entity keys']['needs remote save'];

    $query = db_query("SELECT count($id_key) FROM $table WHERE $needs_remote_save_key = 1");
    $count = $query->fetchField();

    $needs_remote_save_list[] = t("!entity-type: !count entities need remote save", array(
      '!entity-type' => $entity_info['label'],
      '!count'  => $count,
    ));
  }

  $output .= theme('item_list', array(
    'items' => $needs_remote_save_list,
  ));

  return $output;
}
