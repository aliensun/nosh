<?php

/**
 * @file
 * Object used to create a WSClient Operation query.
 */
class WSClientViewsPluginQuery extends views_plugin_query {

  public $operation_name;
  public $service_name;
  public $limit;
  public $offset;
  public $arguments = array();
  public $pager = NULL;

  /**
   * Constructor; Create the basic query object and fill with default values.
   */
  function init($base_table, $base_field, $options) {
    $service_operation = explode(":", $base_table);
    $this->service_name = $service_operation[0];
    $this->operation_name = $service_operation[1];
    $this->base_table = $service_operation[0];
    $this->base_field = $base_field;
    $this->unpack_options($this->options, $options);
  }

  /**
   * Generate a query and a countquery from all of the information supplied
   * to the object.
   *
   * @param $get_count
   *   Provide a countquery if this is true, otherwise provide a normal query.
   */
  function query($get_count = FALSE) {
    return "Querying: " . $this->service_name . " Operation: " .$this->operation_name;
  }

  /**
   * Builds the necessary info to execute the query.
   */
  function build(&$view) {
    // Store the view in the object to be able to use it later.
    $this->view = $view;
    // @TODO
    $view->init_pager();
    // Let the pager modify the query to add limits.
    $this->pager->query();
    $view->build_info['query'] = $this->query();
  }

  /**
   * Set a LIMIT on the query, specifying a maximum number of results.
   */
  function set_limit($limit) {
    $this->limit = $limit;
  }

  /**
   * Set an OFFSET on the query, specifying a number of results to skip
   */
  function set_offset($offset) {
    $this->offset = $offset;
  }

  /**
   * Executes the query and fills the associated view object with according
   * values.
   *
   * Values to set: $view->result, $view->total_rows, $view->execute_time,
   * $view->pager['current_page'].
   *
   * $view->result should contain an array of objects.
   */
  function execute(&$view) {
    $views_result = array();

    $arguments = is_null($view->build_info['query_args']) ? array() : $view->build_info['query_args'];
    $start = microtime(TRUE);

    $results = array();
    if ($service = wsclient_service_load($this->service_name)) {
      // Get the arguments from filters of the view.
      foreach ($this->view->argument as $arg_id => $argument) {
        $arguments[$arg_id] = reset($argument->value);
      }

      $views_configuration = $service->settings['views_configuration'][$this->operation_name];
      if ($views_configuration['parameter']['page'] != 'none') {
        $arguments[$views_configuration['parameter']['page']] = $this->limit;
      }
      if ($views_configuration['parameter']['offset'] != 'none') {
        $arguments[$views_configuration['parameter']['offset']] = $this->offset;
      }
      $results = $service->invoke($this->operation_name, $arguments);
    }

    $views_rows = array();
    // In case if got single result, wrap it with an array for consistency.
    if (is_object($results)) {
      $results = array($results);
    }
    foreach ($results as $result) {
      $views_row = NULL;
      // Attempt to fetch the nested data.
      if (!$views_row = wsclient_views_nested_property_value($views_configuration['result_data'], $result)) {
        // If not found - then just grab full result as views row.
        $views_row = $result;
      }
      $views_rows[] = $views_row;
    }
    // @todo ?Why can't we just count()? Do we need this nested check?
    if (!$count = wsclient_views_nested_property_value($views_configuration['count'], $results)) {
      $count = count($results);
    }

    foreach ($views_rows as $row) {
      $views_row = array();
      // Lowercase the result ids to facilitate matching.
      foreach ($row as $key => $value) {
        $row[strtolower($key)] = $row[$key];
        unset($row[$key]);
      }
      // Generate a field list.
      foreach($view->field as $field_id => $field_info) {
        // Lowercase field id too: again to facilitate matching.
        $field_id_lowercased = strtolower($field_id);
        $value = (string) wsclient_views_nested_property_value("row:" . $field_id_lowercased, $row);
        $views_row[$field_id] = $value;
      }
      $views_result[] = (object) $views_row;
    }

    if ($this->pager->use_pager()) {
      $this->pager->total_items = $count;
      $this->pager->update_page_info();
      $this->pager->post_execute($view->result);
      $view->total_rows = $count;
    }

    $view->result = $views_result;
    $view->execute_time = microtime(TRUE) - $start;
  }

  /**
   * Generates a unique placeholder used in the db query.
   */
  function placeholder($base = 'views') {
    return $base;
  }

  function add_field($table_alias, $real_field) {
    return $real_field;
  }

  function add_where($group, $field, $value = NULL, $operator = NULL) {}

  function ensure_table($table, $relationship = NULL, $join = NULL) {}

  /**
   * Add a complex WHERE clause to the query.
   *
   * The caller is reponsible for ensuring that all fields are fully qualified
   * (TABLE.FIELD) and that the table already exists in the query.
   * Internally the dbtng method "where" is used.
   *
   * @param $group
   *   The WHERE group to add these to; groups are used to create AND/OR
   *   sections. Groups cannot be nested. Use 0 as the default group.
   *   If the group does not yet exist it will be created as an AND group.
   * @param $snippet
   *   The snippet to check. This can be either a column or
   *   a complex expression like "UPPER(table.field) = 'value'"
   * @param $args
   *   An associative array of arguments.
   *
   * @see QueryConditionInterface::where()
   */
  function add_where_expression($group, $snippet, $args = array()) {
    foreach ($args as $arg_key => $arg_val) {
      $stub = $this->service_name . ':' . $this->operation_name . '_';
      $prop = str_replace($stub, "", $arg_key);
      $this->arguments[$prop] = $arg_val;
    }
  }

  /**
   * Render the pager, if necessary.
   */
  function render_pager($exposed_input) {
    if (!empty($this->pager) && $this->pager->use_pager()) {
      return $this->pager->render($exposed_input);
    }
    return '';
  }
}
