<?php

/**
 * @file
 * WSClient Views Admin forms and Theme functions.
 */

/**
 * Form callback to configure views integration for a wsclient service.
 */
function wsclient_views_configuration($form, &$form_state, $service, $operation) {
  $form = array();
  $form['operation'] = array(
    '#tree' => TRUE,
  );

  if (!isset($service->settings['views_configuration'][$operation['name']])) {
    $settings = _wsclient_views_default_views_settings();
  }
  elseif (isset($service->settings['views_configuration'][$operation['name']])) {
    $settings = $service->settings['views_configuration'][$operation['name']];
  }
  if (isset($form_state['values'])) {
    $settings = array_merge($settings, $form_state['values']['operation']);
  }

  $form['operation']['enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Views Support for this Operation'),
    '#description' => t('Enabling views support for this web service operation will expose it\'s result data to views'),
    '#default_value' => $settings['enabled'],
    '#weight' => -100,
  );
  $form['operation']['service'] = array(
    '#value' => $service->name,
  );
  $form['operation']['operation'] = array(
    '#value' => $operation['name'],
  );

  $type = $operation['result']['type'];
  $datatypes = $service->datatypes;
  $result_type = wsclient_views_global_type_name(wsclient_views_raw_type($service->name, $type), $datatypes);
  $data_options = wsclient_views_parse_result_type($result_type, $datatypes);

  $form['operation']['results'] = array(
    '#type' => 'container',
    '#default_value' => $settings['result_data'],
    '#atributes' => array('class' => array('results-section')),
    '#id' => 'wsclient-views-result-row',
    '#theme' => 'wsclient_views_admin_form',
  );
  $form['operation']['results']['result_data'] = array(
    '#title' => t('Result Row Data'),
    '#type' => 'select',
    '#options' => $data_options,
    '#empty_option' => t('Select Data Option'),
    '#description' => t('Choose the result set in the return value that you wish to use as your "row". Prefer <list> types, or the root result set.'),
    '#default_value' => $settings['result_data'],
    '#ajax' => array(
      'wrapper' => 'wsclient-views-result-row',
      'callback' => 'wclient_views_js_replace',
    ),
    '#parents' => array('operation', 'result_data'),
  );
  $form['operation']['results']['result_row'] = array(
    '#title' => t('Result Row'),
    '#type' => 'fieldset',
    '#weight' => 5,
  );

  if (!empty($settings['result_data'])) {
    $properties = wsclient_views_nested_property_info($settings['result_data'], $datatypes);
    $current_values = isset($settings['result_row']) ? $settings['result_row'] : array();

    $master_column_options = array();
    $render_column_options = array();

    foreach ($properties as $id => $prop) {
      $label = isset($prop['label']) ? $prop['label'] : $id;

      // If this property is an integer, it can be the master column
      if (in_array($prop['type'], array('integer', 'text'))) {
        $master_column_options[$id] = $label;
      }

      if (wsclient_views_primitive_datatype($prop['type'])) {
        $render_column_options[$id] = $label;
      }

      $form['operation']['results']['result_row'][$id] = array(
        '#type' => 'container',
        '#attributes' => array('class' => array('result')),
        '#weight' => 10,
        '#id' => 'wsclient-views-result-row-' . $id
      );

      $form['operation']['results']['result_row'][$id]['expand'] = array(
        '#type' => 'checkbox',
        '#title' => 'Expand',
        '#title_display' => 'invisible',
        '#ajax' => array(
          'wrapper' => 'wsclient-views-result-row',
          'callback' => 'wclient_views_js_row_replace',
        ),
        '#parents' => array('operation', 'result_row', $id, 'expand'),
        '#default_value' => isset($current_values[$id]['expand']) ? $current_values[$id]['expand'] : '',
      );

      if (isset($current_values[$id]['expand']) && $current_values[$id]['expand']) {
        $sub_type = wsclient_views_nested_property_info($prop['type'], $datatypes);

        foreach ($sub_type as $ind => $sub) {
          $form['operation']['results']['result_row'][$id . ':' . $ind] = array(
            '#type' => 'container',
            '#attributes' => array('class' => array('result')),
            '#weight' => 10,
          );

          if ($sub_handler = wsclient_views_handler_default($sub['type'])) {
            $render_column_options[$id . ':' . $ind] = $sub['label'];

            $form['operation']['results']['result_row'][$id . ':' . $ind]['handler'] = array(
              '#type' => 'select',
              '#title' => $label,
              '#title_display' => 'invisible',
              '#options' => wsclient_views_renderer_info(),
              '#default_value' => isset($current_values[$id . ':' . $ind]['handler']) ? $current_values[$id . ':' . $ind]['handler'] : $sub_handler,
              '#parents' => array('operation', 'result_row', $id . ':' . $ind, 'handler'),
            );

            $form['operation']['results']['result_row'][$id . ':' . $ind]['expand']['#disabled'] = TRUE;
          }
          else {
            $form['operation']['results']['result_row'][$id . ':' . $ind]['handler'] = array(
              '#markup' => 'struct',
            );
          }

          $form['operation']['results']['result_row'][$id . ':' . $ind]['name']['#markup'] = $label . ": " . $sub['label'];
          $form['operation']['results']['result_row'][$id . ':' . $ind]['type']['#markup'] = $sub['type'];
        }
      }

      if ($handler = wsclient_views_handler_default($prop['type'])) {
        $form['operation']['results']['result_row'][$id]['handler'] = array(
          '#type' => 'select',
          '#title' => $label,
          '#title_display' => 'invisible',
          '#options' => wsclient_views_renderer_info(),
          '#default_value' => isset($current_values[$id]['handler']) ? $current_values[$id]['handler'] : $handler,
          '#parents' => array('operation', 'result_row', $id, 'handler'),
        );

        $form['operation']['results']['result_row'][$id]['expand']['#disabled'] = TRUE;
      }
      else {
        $form['operation']['results']['result_row'][$id]['handler'] = array(
          '#markup' => 'struct',
        );
      }
      $form['operation']['results']['result_row'][$id]['name']['#markup'] = $label;
      $form['operation']['results']['result_row'][$id]['type']['#markup'] = $prop['type'];
    }

    $form['operation']['results']['do_render'] = array(
      '#type' => 'checkboxes',
      '#options' => $render_column_options,
      '#default_value' => isset($settings['render']) ? $settings['result'] : drupal_map_assoc(array_keys($render_column_options)),
      '#parents' => array('operation', 'do_render'),
    );
    $form['operation']['results']['master_column'] = array(
      '#type' => 'radios',
      '#options' => $master_column_options,
      '#default_value' => isset($settings['result']['defaut']) ? $settings['result']['defaut'] : key($master_column_options),
      '#parents' => array('operation', 'master_column'),
    );
  }

  $form['operation']['count'] = array(
    '#title' => t('Count Property'),
    '#type' => 'select',
    '#options' => $data_options,
    '#empty_option' => t('Not Applicable'),
    '#empty_value' => 'none',
    '#description' => t('If provided, choose the count property for the result set.'),
    '#default_value' => $settings['count'],
    '#weight' => 20,
  );

  $parameters = array();
  foreach ($operation['parameter'] as $param_name => $param) {
    if ($param['type'] == 'integer') {
      $parameters[$param_name] = $param['label'];
    }
  }

  $form['operation']['parameter'] = array(
    '#type' => 'fieldset',
    '#title' => 'Parameters',
    '#weight' => 50,
  );

  $form['operation']['parameter']['page'] = array(
    '#title' => t('Pager Parameter (Number of results per page)'),
    '#type' => 'select',
    '#options' => $parameters,
    '#empty_option' => t('Not Applicable'),
    '#empty_value' => 'none',
    '#description' => t('If provided, choose the pager parameter.'),
    '#default_value' => $settings['parameter']['page'],
  );
  $form['operation']['parameter']['offset'] = array(
    '#title' => t('Offset Parameter (Current Page)'),
    '#type' => 'select',
    '#empty_option' => t('Not Applicable'),
    '#empty_value' => 'none',
    '#options' => $parameters,
    '#description' => t('If provided, provide the offset property.'),
    '#default_value' => $settings['parameter']['offset'],
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#name' => 'save',
    '#value' => t('Save Configuration'),
  );

  $form_state['service'] = $service;
  $form_state['operation'] = $operation;

  return $form;
}

/**
 * Form Submit Handler.
 */
function wsclient_views_configuration_submit($form, &$form_state) {
  $service = $form_state['service'];
  $operation = $form_state['operation'];
  $service->settings['views_configuration'][$operation['name']] = $form_state['values']['operation'];
  $service->save();
  drupal_set_message(t('Views Configuration Options Saved'));
  $form_state['rebuild'] = TRUE;
}

function _wsclient_views_default_views_settings() {
  return array(
    'enabled' => FALSE,
    'result_data' => '',
    'result_row' => '',
    'count' => '',
    'parameter' => array(
      'page' => '',
      'offset' => '',
    ),
  );
}

/**
 * Javascript callback to return the updated table.
 */
function wclient_views_js_replace($form, $form_state) {
  return $form['operation']['results'];
}

/**
 * Javascript callback to return the updated table row.
 */
function wclient_views_js_row_replace($form, $form_state) {
  return $form['operation']['results'];
}

function wsclient_views_parse_result_type($type, $datatypes) {
  $result_type = $datatypes[$type];
  wsclient_views_type_info_populate($result_type, $datatypes);

  $result_options = array();

  // Add the root element.
  $result_options[$type] = $datatypes[$type]['label'] . ' (' . $type . ')';

  foreach($datatypes[$type]['property info'] as $op => $op_info) {
    $label = isset($op_info['label']) ? $op_info['label'] : $op;
    $result_options[$type . ':' . $op] = $label . ' (' . $op_info['type'] . ')';
  }

  return $result_options;
}
