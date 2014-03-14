<?php

function wsclient_views_views_data() {
  $data = array();
  $service = entity_load_multiple_by_name('wsclient_service', FALSE);
  // Group Web Services at the service operation level.
  foreach ($service as $name => $info) {
    $views_config = isset($info->settings['views_configuration']) ? $info->settings['views_configuration'] : array();
    // Build a base "table" for each service operation.
    foreach ($info->operations as $op => $op_info) {
      if (!isset($views_config[$op]) || !$views_config[$op]['enabled']) {
        continue;
      }
      $table_name = $name . ':' . $op;
      $data[$table_name]['table']['group']  = "WS: " . $info->label;
      $data[$table_name]['table']['base'] = array(
        'field' => $views_config[$op]['master_column'],
        'title' => "WS: " . $info->label . ", Op: " . $op_info['label'],
        'query class' => 'wsclient_views_query',
        'help' => t('Web Service Client View'),
      );
      $data[$table_name] += wsclient_views_result_type_parse($views_config[$op], $info->datatypes);
      // Parameters.
      if (!empty($op_info['parameter'])) {
        $data[$table_name] += wsclient_views_result_parameter_parse($op_info['parameter']);
      }
    }
  }
  return $data;
}

/**
 * Implementation of hook_views_plugins().
 */
function wsclient_views_views_plugins() {
  return array(
    'query' => array(
      'wsclient_views_query' => array(
        'title' => t('Web Service Client Query'),
        'help' => t('Web Service Client Query Plugin'),
        'handler' => 'WSClientViewsPluginQuery'
      ),
    ),
  );
}
